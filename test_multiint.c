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

	sleep(2);
	kill(pid,SIGINT);/*Send kill to fg process group*/
	sleep(2);
	kill(pid,SIGTERM);
	waitpid(pid,&status,0);
	return 0;
}
