#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <sys/param.h>

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

using namespace std;

void exec_set(string envvar, string val)
{
	if (setenv(envvar.c_str(), val.c_str(), 1) != 0) {
		cout << "setenv error, exit code not zero";
	}
}


int exec_cmd(char *cmd, char *args[], int arg_num)
{
	char *path = (char *)malloc(MAX_LEN * sizeof(char));
	char *home;
	
  	home = getenv("HOME");
	path = getenv("PATH");
	pid_t cpid;  //child process ID for background jobs
	int back_flag = 0;

	//Check for & in command indicating background job
	for (int i = 0; i < sizeof(cmd); i++)
	{
		if (cmd[i] == '&')
		{
			back_flag = 1;
		}
	}

	if ((strcmp(cmd, "ls") == 0) || (strcmp(cmd, "ls&") == 0))
	{		
		strcpy(path, LS_EXEC);
	}	
	else if ((strcmp(cmd, "cd") == 0) || (strcmp(cmd, "cd") == 0))
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
	

	char **exec_args = new char *[MAX_CMDS];
	//contruct argv[] argument for excev(...) function (needs path in first pos)
	exec_args[0] = path;
	for (int j = 1; j < arg_num + 1; j++)
	{
		exec_args[j] = args[j-1];
	}

	cpid = fork();
	//Execute command
	if (back_flag == 0)
	{
		if (cpid == 0)
		{
			execv(path, exec_args);
   			//exit(0);
		} //end if
		else if (cpid > 0)
		{
			wait(NULL);			
		} //end if..else
		else
		{
			printf("Process Failure: ERROR %d\n", errno);
		}
		

	}  
	else  //background execution
	{

		if (cpid == 0)
		{
			execv(path, exec_args);
   			//exit(0);
		} //end if
		else if (cpid > 0)
		{	
			cout << cpid << " running in background\n";	
			wait(NULL);				
		} //end if..else
		else
		{
			printf("Process Failure: ERROR %d\n", errno);
		}
	} //end if..else

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
	cwd = getenv("PWD");


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
    	printf("%s quash> ", cwd);
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
		
		vector<string> cmd_vector;
		
		//TESTING
		printf("%s ", current_cmd);
		cmd_vector.push_back(current_cmd);
		for (int i = 0; i < arg_counter; i++)
		{
			printf("%s ", args[i]);
			cmd_vector.push_back(args[i]);
		}
		printf("\n");
		
		//Exit when user enters "exit" or "quit"
		if ((strcmp(current_cmd, "exit") == 0) || (strcmp(current_cmd, "quit") == 0))
		{
			break;
		}

		//Execute command
		exec_cmd(current_cmd, args, arg_counter);

		//execute(cmd_vector);
	} //end while

	//free(*args);

  return 0;

} //end main
