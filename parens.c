#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mylimits.h"
#include <string.h>
#include <assert.h>


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



void parenthesis(char* head,char* tail,void (*peval)(char *))
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
	peval(cmd);
}


//-------------------------------------------------------------------------------------
//Call this parser right after you read in a command.
//-------------------------------------------------------------------------------------
int parseparenthesis(char* cmd, void (*peval)(char *))
{
	int l=strlen(cmd);
	int count,i;
	count=0;
    int has_parens=0;
	for (i=0;i<l;i++)
	{
		if (cmd[i]=='('){
            count++;
            has_parens=1;
        }
		if (cmd[i]==')') count--;
		if (count < 0) return -1;
	}
    if(has_parens == 0){
        peval(cmd);
        return 0;
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
			parenthesis(stack[stackpointer],current,peval);
			erase(stack[stackpointer],current);
		}
		current++;
	}
    //printf("%s\n",cmd);
	return 0;
}


/*
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
		if(parseparenthesis(buf,eval)!=0) printf("Parenthesis syntax error\n");
	}
  exit(0);
}
*/
