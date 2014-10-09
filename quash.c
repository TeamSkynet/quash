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


int exec_cmd(char *cmd, char *args[], int arg_num)
{
	char *path = (char *)malloc(MAX_LEN * sizeof(char));
	//char *home;
	
  	//home = getenv("HOME");
	//path = home;
	pid_t cpid;

	if (strcmp(cmd, "ls") == 0)
	{		
		strcpy(path, LS_EXEC);
	}
	/*
	else if (strcmp(cmd, "cd") == 0)
	{
		if (args[0] != NULL)
		{		
			strcpy(path, args[0]);
		}
		else
		{
			strcpy(path, home);
		}
		chdir(path);
	} //end if..else if..else
	*/

	cpid = fork();

	if (cpid >= 0)
	{
		if (cpid == 0)
		{

			execv(path, args);
   			exit(0);
		} //end if
	} 
	else
	{
		printf("Process Failure: ERROR %d\n", errno);
		return 1;
	}

	return 0;
}


//*******Main Program*******

int main(int argc, char *argv[])
{
	//Environment variables
	char *home, *host, *path, *cwd;

 	home = getenv("HOME");
  	host = getenv("HOSTNAME");
	path = getenv("PATH");
	//cwd = getenv("PWD");


	//Other variables
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

	//Initialize arguments array
	for (int i = 0; i < MAX_LEN; i++)
	{
		args[i] = (char *) malloc(5);
		//args[i] = (char *) NULL;
	}

	while(1) 
	{
    	printf("%s quash> ", home);
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
			cmd = (strtok(NULL, DELIMS));

			if (cmd != NULL)
			{
				args[arg_counter] = cmd;
				if (strcmp(args[arg_counter], "|") == 0)
				{
					pipe_counter++;
				}	
				arg_counter++;				
			}
						
		} //end while

		//Terminate args with NULL char
		args[arg_counter + 1] = NULL;
		
		//TESTING
		printf("%s ", current_cmd);
		for (int i = 0; i < arg_counter; i++)
		{
			printf("%s ", args[i]);
		}
		printf("\n");

		if ((strcmp(current_cmd, "exit") == 0) || (strcmp(current_cmd, "quit") == 0))
		{
			break;
		}

		//Execute command
		exec_cmd(current_cmd, args, arg_counter);
		
	} //end while

	//free(*args);

  return 0;

} //end main
