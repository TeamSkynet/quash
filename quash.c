#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_LEN 64
#define MAX_JOBS 32
#define MAX_CMDS 10
#define DELIMS " \t\r\n"

#define BASH_EXEC  "/bin/bash"
#define FIND_EXEC  "/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC  "/bin/grep"
#define SORT_EXEC  "/bin/sort"
#define HEAD_EXEC  "/usr/bin/head"
#define LS_EXEC    "/usr/bin/ls"


//NOT USED YET
void create_cmd_stack(char *args[], int pipe_num, int arg_num)
{

	int fds[2 * pipe_num];
	pid_t pid;
	char *parse_buffer[MAX_LEN];
	int pipe_indexes[10];
	
	//Initialize pipe_indexes
	for (int i = 0; i < 10; i++)
	{
		pipe_indexes[i] = 0;
	}

	//command struct
	
	struct command
	{
		char cmd[10];
		char *args[10];
		int arg_num;
	};
	

	//Create and initialize command_stack
	struct command command_stack[MAX_CMDS];
	struct command c;
	strcpy(c.cmd, "\0");
	c.arg_num = 0;

	for (int j = 0; j < 10; j++)
	{
		c.args[j] = (char*) malloc(5);
		strcpy(c.args[j], "\0");		
	}
	
	for (int i = 0; i < MAX_CMDS + 1; i++)
	{
		command_stack[i] = c;
	}
	
	//Read in from args, separate commands from args and pipes
	
	//Collect index positions of pipes in args
	int j = 0;
	for (int i = 0; i < arg_num; i++)
	{
		//printf("args[%d]= %s\n", i, args[i]);
		if (strcmp(args[i], "|") == 0)
		{
			pipe_indexes[j] = i;
			j++;
		}		
	} //end for	

	//Count arguments for commands
	for (int i = 0; i < pipe_num; i++)
	{
		command_stack[i].arg_num = pipe_indexes[i] - 1;
		printf("command_stack[%d].arg_num = %d\n", i, command_stack[i].arg_num);
	}

	//Case 1: no pipes
	if (pipe_num < 1)
	{
		strcpy(command_stack[0].cmd, args[0]);	
		for (int i = 1; i < arg_num; i++)
		{
			command_stack[0].args[i-1] = args[i];
			//command_stack[0].arg_num++;
		} //end for
	} 
	else
	{
	//Case 2: with pipes
	//Use pipe_indexes to read in commands and arguments
		char *arg_loop[10];
		for (int j = 0; j < 10; j++)
		{
			arg_loop[j] = (char *) malloc(5);
		}
		
		for (int i = 0; i < pipe_num; i++)
		{
			strcpy(command_stack[i].cmd, args[pipe_indexes[i] - (command_stack[i].arg_num + 1)]);
			for(int k = 0; k < command_stack[i].arg_num; k++)
			{
				arg_loop[k] = args[k+1];
			} //end inner for
					
			for (int q = 0; q < 10; q++)
			{
				command_stack[i].args[q] = arg_loop[q];				
			} //end inner for		
		} //end outer for
	} //end if

	//FIX - Read in last command if pipe used
	
	if (pipe_num > 0)
	{
		strcpy(command_stack[pipe_num].cmd, args[pipe_indexes[pipe_num - 1] + 1]);
		for (int i = (pipe_indexes[pipe_num -1]) + 2; i < arg_num; i++)
		{	
			strcpy(command_stack[pipe_num].args[i-((pipe_indexes[pipe_num -1]) + 2)], args[i]);
			command_stack[pipe_num].arg_num++;
		}
	}  

	//TESTING - FIX Seg Fault when k > 1
	for (int i = 0; i < 10; i++)
	{
		printf("command_stack[%d] = %s ", i, command_stack[i].cmd);
		for (int k = 0; k < command_stack[i].arg_num; k++)
		{
			printf("%s ",command_stack[i].args[k]);
		}
		printf("\n");
	} // end for
	/*
	//Pipe creation error handling
	for (int i = 0; i < pipe_num; i++)
	{
		if(pipe(fds + 2 * i) < 0)
		{
			fprintf(stderr, "Process %d encountered an error. ERROR%d\n", i, errno);
			exit(EXIT_FAILURE);
		}
	} //end for
	*/
	
} //end exec_cmd


int main(int argc, char *argv[])
{
	char *cmd;  //command and argument buffer
	char *current_cmd;  //current command for execution
	char *args[MAX_LEN];  //argument array
	int arg_counter;
	char ln[MAX_LEN];
	int pipe_counter;

	struct job
	{
		int jobid;
		pid_t pid;
		char *command;
	};

	struct command
	{
		char cmd[10];
		char *args[10];
	};

	job jobs[MAX_JOBS];

	while(1) 
	{
    	printf("quash> ");
    	if (!fgets(ln, MAX_LEN, stdin))
    	{
      		break;
    	}
		
		//Read in command and arguments
		cmd = strtok(ln, DELIMS);
		current_cmd = cmd;
		arg_counter = 0;
		pipe_counter = 0;
		while (cmd != NULL)
		{	
			if (cmd != NULL)
			{
				args[arg_counter] = cmd;
				if (strcmp(args[arg_counter], "|") == 0)
				{
					pipe_counter++;
				}	
				arg_counter++;
				
			}	
			cmd = (strtok(NULL, DELIMS));
			
		} //end while
		
		//Execute command
		create_cmd_stack(args, pipe_counter, arg_counter);
		
	} //end while

  return 0;

} //end main
