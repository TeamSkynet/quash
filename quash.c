#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_LEN 64
#define MAX_JOBS 32
#define DELIMS " \t\r\n"

#define BASH_EXEC  "/bin/bash"
#define FIND_EXEC  "/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC  "/bin/grep"
#define SORT_EXEC  "/bin/sort"
#define HEAD_EXEC  "/usr/bin/head"
#define LS_EXEC    "/usr/bin/ls"


//Executes the ls command
void exec_ls()
{
	execl(LS_EXEC, LS_EXEC, (char *) NULL);

} //exec_ls

void exec_cmd(char *args[], int pipe_num, int arg_num)
{

	int fds[2 * pipe_num];
	pid_t pid;
	char *parse_buffer[MAX_LEN];
	int pipe_indexes[10];

	struct command
	{
		char cmd[10];
		char *args[10];
	};
	
	struct command command_stack[5];


	//Pipe creation error handling
	for (int i = 0; i < pipe_num; i++)
	{
		if(pipe(fds + 2 * i) < 0)
		{
			fprintf(stderr, "Process %d encountered an error. ERROR%d", i, errno);
			exit(EXIT_FAILURE);
		}
	} //end for

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
	//printf("pipe_indexes = [%d, %d, %d]\n", pipe_indexes[0], pipe_indexes[1], pipe_indexes[2]);
	
	//Case 1: no pipes
	if (pipe_num < 1)
	{
		struct command command_stack[0];  //REMOVE
		strcpy(command_stack[0].cmd, args[0]);	
		for (int i = 1; i < arg_num; i++)
		{
			command_stack[0].args[i-1] = args[i];
		} //end for
	}

	//Case 2: with pipes
	//Use pipe_indexes to read in commands and arguments
	char *arg_loop[10];
	for (int i = 0; i < pipe_num; i++)
	{
		struct command command_stack[i];  //REMOVE
		printf("args[i] = %s\n", args[i]);
		strcpy(command_stack[i].cmd, args[i]);		
		for(int k = 0; k < pipe_indexes[i] - 1; k++)
		{
			arg_loop[k] = args[k+1];			
		} //end inner for
		
		for (int q = 0; q < 10; q++)
		{
			command_stack[i].args[q] = arg_loop[q];
		} //end inner for
			
	} //end outer for

	//TESTING
	/*
	for (int i = 0; i < 10; i++)
	{
		printf("command_stack[%d]= %s", command_stack[i].cmd);
		for (int k = 0; k < 10; k++)
		{
			printf("%s ", command_stack[i].args[k]);
		}
		printf("\n");
	} */
	
	//Read in last command if pipe used
	/*
	if (pipe_num > 0)
	{
		command_stack.cmd = args[pipe_indexes[pipe_num -1];
		while (args != NULL)
		{
			command_stack.args
		}
	}
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


		//printf("Command echo: %s\n", current_cmd);
		/*
		for (int i=0; i < arg_counter; i++)
		{
			printf("Arg echo: %s\t, pipe_counter = %d\n", args[i], pipe_counter);
		}		*/
		
		//Execute command
		exec_cmd(args, pipe_counter, arg_counter);
		
	} //end while

  return 0;

} //end main
