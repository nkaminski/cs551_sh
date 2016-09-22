/*
 * 551_tsh - A far more advanced tiny shell program with job control
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include "mylimits.h"
#include "builtins.h"
#include "jobs.h"
#include "alias.h"
#include "parens.h"
#include "signals.h"
#include "profile.h"


/* Global variables */
extern char **environ;      /* defined in libc */
const char prompt[] = " > ";    /* command line prompt (DO NOT CHANGE) */

const char *path_dirs[] = {"/bin", "/usr/bin"};
const char n_path_dirs = 2;
char aliasesfile[PATH_MAX];
dlist_head_t alias_list;

char fullprompt[sizeof(prompt)+PATH_MAX];
int strikes = 0;
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */


struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */

/* Function prototypes */

void eval(char *cmdline);
void eval2(char *cmdline);
int parseline(const char *cmdline, char **argv);
void usage(void);
void unix_error(char *msg);
void app_error(char *msg);

/*
 * main - The shell's main routine
 */
int main(int argc, char **argv)
{
    getcwd(aliasesfile, PATH_MAX-9);
	init_dir(); //initialize working directory
	char c;
	char cmdline[MAXLINE];
	int emit_prompt = 1; /* emit prompt (default) */

	/* Redirect stderr to stdout (so that driver will get all output
	 * on the pipe connected to stdout) */
	dup2(1, 2);

	/* Parse the command line */
	while ((c = getopt(argc, argv, "hvp")) != EOF) {
		switch (c) {
			case 'h':             /* print help message */
				usage();
				break;
			case 'v':             /* emit additional diagnostic info */
				verbose = 1;
				break;
			case 'p':             /* don't print a prompt */
				emit_prompt = 0;  /* handy for automatic testing */
				break;
			default:
				usage();
		}
	}

	/* Install the signal handlers */

	Signal(SIGINT,  sigint_handler);   /* ctrl-c */
	Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
	Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

	/* This one provides a clean way to kill the shell */
	Signal(SIGQUIT, sigquit_handler);

	/* Initialize the job list */
	initjobs(jobs);
    

    /* Load aliases file */
    strcat(aliasesfile,"/.aliases");
    load_aliases(aliasesfile);
    
	/* Execute the shell's read/eval loop */
	while (1) {

		/* Read command line */
		if (emit_prompt) {
            getcwd(fullprompt, PATH_MAX);
            strcat(fullprompt, prompt);
			printf("%s", fullprompt);
			fflush(stdout);
		}
		if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
			app_error("fgets error");
		if (feof(stdin)) { /* End of file (ctrl-d) */
			fflush(stdout);
            save_aliases(aliasesfile);
			exit(0);
		}

		/* Evaluate the command line */
        eval(cmdline);
        /* reset control-c counter */
            strikes=0;
		fflush(stdout);
	}

	exit(0); /* control never reaches here */
}
/*
 * eval - Evaluate the command line that the user has just typed in
 *
 * If the user has requested a built-in command (exit, jobs, cd, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.
 */
void eval(char *cmdline){
	char *argv[MAXARGS];
    /* check for the alias command first */
    parseline(cmdline,argv);
    if(argv[0] == NULL)
        return;
    if(strcmp(argv[0],"alias")==0){
        set_alias(argv);
        return;
    }
    /* otherwise do alias substitutions and handle as normal */
    /* alias resolution */
    resolve_alias(cmdline);
	if(parseparenthesis(cmdline,eval2) == -1){
        printf("Error: Unbalanced parenthesis");
    } 
}
void eval2(char *cmdline)
{
	int jt;
	pid_t pid;
	struct job_t *job;
	char *argv[MAXARGS];
	sigset_t mask;
    char *tok_s;
    char *saveptr_s;
    char cmdcpy[MAXLINE];
    char cmdcpy2[MAXLINE];
    char fqpath[PATH_MAX];
    int nowait;
    strcpy(cmdcpy2,cmdline);
   /* leading or trailing space removal */
    while(cmdcpy2[0] != '\0' && cmdcpy2[0] == ' '){
        memmove(cmdcpy2, cmdcpy2+1, strlen(cmdcpy2));
    }
    while(cmdcpy2[0] != '\0' && cmdcpy2[strlen(cmdcpy2)-1] == ' '){
        cmdcpy2[strlen(cmdcpy2)-1] = '\0';
    }
    /* leading or trailing comma removal */
    while(cmdcpy2[0] != '\0' && cmdcpy2[0] == ','){
        memmove(cmdcpy2, cmdcpy2+1, strlen(cmdcpy2));
    }
    while(cmdcpy2[0] != '\0' && cmdcpy2[strlen(cmdcpy2)-1] == ','){
        cmdcpy2[strlen(cmdcpy2)-1] = '\0';
    }
    /* alias resolution */
    resolve_alias(cmdcpy2);

    strncpy(cmdcpy,cmdcpy2,MAXLINE-1);
    tok_s = strtok_r(cmdcpy, ";&,", &saveptr_s);
    while(tok_s != NULL){
        if(cmdcpy2[tok_s-cmdcpy+strlen(tok_s)] == '&')
            jt=1;
        else
            jt=0;
        /* do we intend to run multiple foreground jobs? */
        if(cmdcpy2[tok_s-cmdcpy+strlen(tok_s)] == ',')
            nowait=1;
        else
            nowait=0;
        parseline(tok_s,argv);
        if(!argv[0])
            return;
        sigemptyset (&mask);
        sigaddset (&mask, SIGCHLD);
        if(strcmp(argv[0],"exit")==0){
            save_aliases(aliasesfile);
            exit(0);
        }
        else if(strcmp(argv[0],"bg")==0 || strcmp(argv[0],"fg")==0){
            do_bgfg(argv);
        }
        else if(strcmp(argv[0],"aliases")==0){
            print_aliases(argv);
        }
        else if(strcmp(argv[0],"cd")==0){
            do_cd(argv);
        }
        else if(strcmp(argv[0], "kill")==0 || strcmp(argv[0],"jobs")==0){
            builtin_cmd(argv);
        }
        else{
            strcpy(fqpath, argv[0]);
            if(resolve_path(fqpath)<0){
                printf("%s does not exist on PATH\n", fqpath);
                goto jobwait;
            }
            //printf("path resolved to %s\n", fqpath);
            sigprocmask(SIG_BLOCK,&mask,NULL);
            if((pid = fork())==0){
                setpgrp();
                sigprocmask(SIG_UNBLOCK,&mask,NULL);
                //printf("child\n");
                execve(fqpath,argv,environ);
                printf("%s: Command not found\n",argv[0]);
                exit(0);
            }
            //printf("parent\n");
            if(jt){
                addjob(jobs, pid, BG, tok_s);
                job=getjobpid(jobs,pid);
                printf("[%i] (%i) %s",job->jid,job->pid,job->cmdline);
            } else{
                addjob(jobs, pid, FG, tok_s);
            }
            sigprocmask(SIG_UNBLOCK,&mask,NULL);
        }
        //printf("nowait %d\n",nowait);
jobwait:
        while((pid=fgpid(jobs)) && (nowait == 0)){
            //printf("waiting %i\n",pid);
            waitfg(pid);
        }
        tok_s = strtok_r(NULL,";&,",&saveptr_s);
    }
}

/*
 * parseline - Parse the command line and build the argv array.
 *
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.
 */
int parseline(const char *cmdline, char **argv)
{
    static char array[MAXLINE]; /* holds local copy of command line */
	char *buf = array;          /* ptr that traverses command line */
	char *delim;                /* points to first space delimiter */
	int argc;                   /* number of args */
	int bg;                     /* background job? */

	strcpy(buf, cmdline);
    if(buf[strlen(buf)-1] == '\n')
	    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
	else
        buf[strlen(buf)] = ' ';  /* stick a space on the end */
	while (*buf && (*buf == ' ')) /* ignore leading spaces */
		buf++;

	/* Build the argv list */
	argc = 0;
	if (*buf == '\'') {
		buf++;
		delim = strchr(buf, '\'');
	}
	else {
		delim = strchr(buf, ' ');
	}

	while (delim) {
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while (*buf && (*buf == ' ')) /* ignore spaces */
			buf++;

		if (*buf == '\'') {
			buf++;
			delim = strchr(buf, '\'');
		}
		else {
			delim = strchr(buf, ' ');
		}
	}
	argv[argc] = NULL;

	if (argc == 0)  /* ignore blank line */
		return 1;

	/* should the job run in the background? */
	if ((bg = (*argv[argc-1] == '&')) != 0) {
		argv[--argc] = NULL;
	}
	return bg;
}
