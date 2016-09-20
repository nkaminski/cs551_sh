#include <stdio.h>
#include <stdlib.h>
#include "jobs.h"
#include "signals.h"
#include "alias.h"
#include "util.h"
/*****************
 * Signal handlers
 *****************/
/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler)
{
	struct sigaction action, old_action;

	action.sa_handler = handler;
	sigemptyset(&action.sa_mask); /* block sigs of type being handled */
	action.sa_flags = SA_RESTART; /* restart syscalls if possible */

	if (sigaction(signum, &action, &old_action) < 0)
		unix_error("Signal error");
	return (old_action.sa_handler);
}
/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.
 */
void sigchld_handler(int sig)
{
	pid_t pid;
	int stat;
	struct job_t *job;
	sigset_t mask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGCHLD);
	sigprocmask(SIG_BLOCK,&mask,NULL);
	while((pid=(waitpid(-1,&stat,WNOHANG | WUNTRACED))) > 0){
		job=getjobpid(jobs,pid);
		if(WIFSIGNALED(stat)){
			printf("Job [%i] (%i) terminated by signal %i\n",job->jid,pid,WTERMSIG(stat));
		}
		if(WIFEXITED(stat) || WIFSIGNALED(stat)){
			deletejob(jobs,pid);
		} else if(WIFSTOPPED(stat)){
			printf("Job [%i] (%i) stopped by signal %i\n",job->jid,pid,WSTOPSIG(stat));
			job->state=ST;
		}
	}
	sigprocmask(SIG_UNBLOCK,&mask,NULL);
	return;
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job or prompt to quit the shell.
 */
void sigint_handler(int sig)
{
	pid_t fgp;
	sigset_t mask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGINT);
	sigprocmask(SIG_BLOCK,&mask,NULL);
	fgp=fgpid(jobs);
	//printf("int handler fgpid: %i\n",fgp);
	if(fgp>0){
		kill(-fgp,sig);
	} else {
        strikes++;
        if(strikes==2){
            save_aliases(aliasesfile);
            exit(0);
        }
        printf("\nAre you sure? CTRL-C again to quit.\n");
    }
	sigprocmask(SIG_UNBLOCK,&mask,NULL);
	return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void sigtstp_handler(int sig)
{
	pid_t fgp;
	struct job_t *fgj;
	sigset_t mask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGTSTP);
	sigprocmask(SIG_BLOCK,&mask,NULL);
	fgp=fgpid(jobs);
	if(fgp){
		fgj=getjobpid(jobs,fgp);
		kill(-fgp,sig);
		//wait for job to stop before returning
		while(fgj->state!=ST){
			sleep(1);
		}

	}
	sigprocmask(SIG_UNBLOCK,&mask,NULL);
	return;
}
/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig)
{
	printf("Terminating after receipt of SIGQUIT signal\n");
	save_aliases(aliasesfile);
    exit(1);
}
/*********************
 * End signal handlers
 *********************/
