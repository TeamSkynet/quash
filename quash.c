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

void exec_cmd(char **args, int pipe_num)
{
	//const int commands = pipe_num + 1;
	//int i = 0;

	int fds[2 * pipe_num];
	pid_t pid;
	//int place_holder;
	//int next_command_pos[MAX_LEN];
	char *parse_buffer[MAX_LEN];
	int pipe_indexes[10];
	struct command
	{
		char *cmd;
		char *args[MAX_LEN];
	};
	
	command command_stack[10];

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
	for (int i = 0; args[i] != NULL; i++)
	{
		if (strcmp(args[i], "|") == 0)
		{
			pipe_indexes[j] = i;
			j++;
		}		
	} //end for	
	printf("pipe_indexes = [%d, %d, %d]\n", pipe_indexes[0], pipe_indexes[1], pipe_indexes[2]);




	
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
		exec_cmd(args, pipe_counter);
		
	} //end while

  return 0;

} //end main
