#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mylimits.h"
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>


void eval (char*);


//-------------------------------------------------------------------------------------
//Replace a substring with whitespaces.
//-------------------------------------------------------------------------------------
void erase(char* head,char* tail)
{
	char* current=head;
	while (current<=tail)
	{
		*current=' ';
		current++;
	}
}



void parenthesis(char* head,char* tail)
{
    int l=tail-head-1;
    int i=0;
	char  cmd[l+1];
    char *current=head;
	for (i=0;i<l;i++)
    {
        current++;
        cmd[i]=*current;
    }
    cmd[i]=0x00;
	//-------------------------------------------------------------------------------------	
	//This is where your eval() will be called. Just uncomment the next line.
	//-------------------------------------------------------------------------------------
	eval(cmd);
}

void eval(char *cmd){
    printf("%s\n",cmd);
}


//-------------------------------------------------------------------------------------
//Call this parser right after you read in a command.
//-------------------------------------------------------------------------------------
int parseparenthesis(char* cmd)
{
	int l=strlen(cmd);
	int count,i;
	count=0;
	for (i=0;i<l;i++)
	{
		if (cmd[i]=='(') count++;
		if (cmd[i]==')') count--;
		if (count<0) return -1;
	}
	char *current,*tail;
	static char *stack[MAXLINE];
	static int stackpointer=0;
	current=cmd;
	tail=cmd+l;
	while (current<tail)
	{
		if (*current=='(')
		{
			stack[stackpointer]=current;
			stackpointer++;
		}
		if (*current==')')
		{
			stackpointer--;
			parenthesis(stack[stackpointer],current);
			erase(stack[stackpointer],current);
		}
		current++;
	}
    printf("%s\n",cmd);
	return 0;
}




//-------------------------------------------------------------------------------------
//Below is for my testing. All you need to add to your shell are the 4 functions above.
//-------------------------------------------------------------------------------------


int
getcmd(char *buf, int nbuf)
{
//  printf("getcmd called\n");
  if (isatty(fileno(stdin)))
    fprintf(stdout, "$ ");
  memset(buf, 0, nbuf);
  fgets(buf, nbuf, stdin);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
main(void)
{
	static char buf[100];
	int fd, r;
	// Read and run input commands.
	while(getcmd(buf, sizeof(buf)) >= 0)
	{
		if(parseparenthesis(buf)!=0) printf("Parenthesis syntax error\n");
	}
  exit(0);
}
