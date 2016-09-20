#include "jobs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
/*
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.
 */
int builtin_cmd(char **argv)
{
	struct job_t *jr;
	if(strcmp(argv[0],"jobs")==0){
		listjobs(jobs);
	} else if(strcmp(argv[0],"kill")==0 && argv[1]){
		jr=getjobarg(jobs,argv[1]);
		kill(jr->pid,SIGINT);
	}
	return 0;     /* not a builtin command */
}

/*
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv)
{
	struct job_t *jr;
	if(argv[1]==NULL){
		printf("%s command requires a PID or %%jobid argument\n",argv[0]);
		return;
	}
	if(atoi(argv[1])!=0){
		goto ok;
	}
	if(argv[1][0]=='%'){
		if(atoi(argv[1]+1)!=0){
			goto ok;
		}
	}
	printf("%s: argument must be a PID or %%jobid\n",argv[0]);
	return;
ok:
	jr=getjobarg(jobs,argv[1]);
	if(!jr)
		return;
	if(strcmp(argv[0],"fg")==0){
		if(jr->state==ST || jr->state==BG){
			if(jr->state==ST)
				kill(-1*(jr->pid),SIGCONT);
			jr->state=FG;
		}
		waitfg(jr->pid);
	} else if(strcmp(argv[0],"bg")==0){
		if(jr->state==ST){
			kill(-1*(jr->pid),SIGCONT);
		}
			jr->state=BG;
			printf("[%i] (%i) %s",jr->jid,jr->pid,jr->cmdline);
	}

	return;
}
/*
 * do_cd - Execute the builtin cd command
 */
void do_cd(char **argv)
{
	if(argv[1]==NULL){
		printf("%s command requires a directory path as an argument\n",argv[0]);
		return;
	}
    if(chdir(argv[1]) == -1){
        perror("Failed to change directory");
    }
	return;
}

