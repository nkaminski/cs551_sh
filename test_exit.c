/* 
 * tsh - A tiny shell program with job control
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    dup2(1, 2);
	char *tshargv[] = {"tsh",NULL};
	int pid;
	int status;
	if((pid = fork()) == 0)
	{
		if(execve("./tsh",tshargv,environ) < 0)
		{
			printf("%s: Command not found\n",argv[0]);
			exit(0);
		}
	}

	sleep(1);
	kill(pid,SIGINT);/*Send kill to fg process group*/
	sleep(1);
	kill(pid,SIGINT);
	waitpid(pid,&status,0);
	return 0;
}
