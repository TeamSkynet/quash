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

typedef struct comm
{
	char cmd[10];
	char *args[MAX_CMDS];
	int arg_num;
} comm;

void exec_set(string envvar, string val)
{
	if (setenv(envvar.c_str(), val.c_str(), 1) != 0) {
		cout << "setenv error, exit code not zero";
	}
}

/*Name: exec_cmd
  Description:  Executes a single (non-piped) command
  Arguments:
			cmd: The command to be executed.
			args: The arguments to be passed to this command.
			arg_num:  The number of arguments.
  Returns: 0, if completes successfully.
*/
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
	else if (strcmp(cmd, "sort") == 0)
	{
		strcpy(path, SORT_EXEC);
	}
	else if (strcmp(cmd, "head") == 0)
	{
		strcpy(path, HEAD_EXEC);
	}
	else if (strcmp(cmd, "grep") == 0)
	{
		strcpy(path, GREP_EXEC);
	}	
	else if ((strcmp(cmd, "cd") == 0) || (strcmp(cmd, "cd&") == 0))
	{  //Not quite working yet...doesn't return to main after running cd
		if (args[0] != NULL)
		{		
			strcpy(path, args[0]);
		}
		else
		{
			strcpy(path, home);
		}

		//Change directory to path
		cpid = fork();
		if (cpid == 0)
		{
			chdir(path);
   			//exit(0);
		} //end if
		else if (cpid > 0)
		{
			wait(NULL);			
		} //end if..else
		else
		{
			printf("cd: Process Failure: ERROR %d\n", errno);
		}
		
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

/*Name: exec_piped_cmd
  Description:  Executes a piped command sequence
  Arguments:
			cmds: The command to be executed, in the form of a comm (struct) array.
				  The array contains both the commands and arguments in the individual
				  comm objects.  Limit two commands.
  Returns: 0, if completes successfully.
*/
int exec_piped_cmd(struct comm cmd1, struct comm cmd2)
{
		//TESTING
		/*
		printf("In exec_piped_cmd\n");
		printf("%s ", cmd1.cmd);

		for (int i = 0; i < cmd1.arg_num; i++)
		{
			printf("%s ", cmd1.args[i]);
		}
		printf("\n");

		printf("%s ", cmd2.cmd);

		for (int i = 0; i < cmd2.arg_num; i++)
		{
			printf("%s ", cmd2.args[i]);
		}
		printf("\n");
		*/

		

	return 0;
}


//*******Main Program*******

int main(int argc, char **argv, char **envp)
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
	char *ln = new char[MAX_LEN];
	char *ln1 = new char[MAX_LEN];
	char *ln2 = new char[MAX_LEN];
	int pipe_counter;
	int command_count;

	typedef struct job
	{
		int jobid;
		pid_t pid;
		char *command;
	} job;


	job jobs[MAX_JOBS];  //Job stack



	comm* comms = new comm[2];  //command stack
	for (int i = 0; i < 2; i++)
	{
		comms[i].arg_num = 0;
	}

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
		
		
		pipe_counter = 0;
		command_count = 1;
		int ln_pos = 0;
		//Read in command and arguments
		for(int i = 0; i < strlen(ln); i++)  //Check for pipe
		{
			ln_pos = i;
			if (ln[i] == '|')
			{
				pipe_counter = 1;
				break;
			}			
		}  //end for
		

		if (pipe_counter > 0)  //pipe present
		{
			strncpy(ln1, ln, ln_pos - 1);
			ln1[ln_pos - 1] = '\0';
			strncpy(ln2, ln + ln_pos + 2, strlen(ln) - ln_pos);
			ln2[strlen(ln) - ln_pos] = '\0';
			command_count = 2;
		}
		else  //no pipe
		{
			strncpy(ln1, ln, strlen(ln));
			ln1[strlen(ln)] = '\0';
			strcpy(ln2, "\0");
		}
		
		//printf("ln1 = %s\n", ln1);
		//printf("ln2 = %s\n", ln2);

		//Parse first command string
		cmd = strtok(ln1, DELIMS);
		strcpy(comms[0].cmd, cmd);
		comms[0].arg_num = 0;
		while (cmd != NULL)
		{	
			cmd = (strtok(NULL, DELIMS));

			if (cmd != NULL)
			{
				comms[0].arg_num++;
				printf("arg_num = %d\n", comms[0].arg_num);
				comms[0].args[comms[0].arg_num - 1] = cmd;
			}
						
		} //end while

		//Terminate args with NULL char
		comms[0].args[comms[0].arg_num] = NULL;
		
		
		//Parse second command string
		if (command_count > 1)
		{
			cmd = strtok(ln2, DELIMS);
			strcpy(comms[1].cmd, cmd);
			comms[1].arg_num = 0;
			while (cmd != NULL)
			{	
				cmd = (strtok(NULL, DELIMS));

				if (cmd != NULL)
				{
					comms[1].arg_num++;	
					comms[1].args[comms[1].arg_num - 1] = cmd;		
				}
						
			} //end while

			//Terminate args with NULL char
			comms[1].args[comms[1].arg_num] = NULL;
			} //end if

		//vector<string> cmd_vector;
		
		//TESTING
		printf("%s ", comms[0].cmd);
		//cmd_vector.push_back(current_cmd);
		for (int i = 0; i < comms[0].arg_num; i++)
		{
			printf("%s ", comms[0].args[i]);
			//cmd_vector.push_back(args[i]);
		}
		printf("\n");

		if (command_count > 1)
		{
			printf("%s ", comms[1].cmd);
			//cmd_vector.push_back(current_cmd);
			for (int i = 0; i < comms[1].arg_num; i++)
			{
				printf("%s ", comms[1].args[i]);
				//cmd_vector.push_back(args[i]);
			}
			printf("\n");
		} //end if

		
		
		//Exit when user enters "exit" or "quit"
		if ((strcmp(comms[0].cmd, "exit") == 0) || (strcmp(comms[0].cmd, "quit") == 0))
		{
			break;
		}  //end if

		//Execute command
		if (pipe_counter > 0)  //execute piped commands
		{
			exec_piped_cmd(comms[0], comms[1]);
		}
		else  //single command execution
		{
			exec_cmd(comms[0].cmd, comms[0].args, comms[0].arg_num);
		} //end if..else
		
		cwd = getenv("PWD");
		//execute(cmd_vector);
	} //end while

	//free(*args);
	delete[] ln;
	delete[] ln1;
	delete[] ln2;
	delete[] comms;

  return 0;

} //end main
