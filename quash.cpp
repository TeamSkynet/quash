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

void execute(vector<string> command)
{
	pid_t child_pid;	

	if(command.at(0) == "cd") {
		string path = "";
		
		// Create new path
		for (int j = 1; j < command.size(); ++j) {
			path += command.at(j) + " ";
		}

		chdir(path.c_str());
		
		char *test_path=NULL;
		size_t size;
		test_path=getcwd(test_path,size);
		cout << "\nCurrent Path: " << test_path << endl << endl;
		
		return;
	}
	if(command.at(0) == "set") {
		string path = "";
		for (int j = 1; j < command.size(); ++j) {
			path += command.at(j);
			
			if (j < command.size()-1)
				path += " ";
		}
		
		exec_set(path.substr(0, path.find("=")), path.substr(path.find("=")+1));
		
		return;
	}

	
	string cmd_path(command.at(0));
	
	
	// If command is not specified as absolute path, search directories in 
	// PATH environment variable
	if(cmd_path.at(0) != '/') {
	
		// Parse PATH into array, each index a directory
		vector<string> paths;
		
		// path_var_str = "";
		
		const char* raw_paths = getenv("PATH");
		
		//if (path_var != NULL) {
		//	string path_var_str = path_var;
		//}
		
		string str_paths = raw_paths?raw_paths:"";
		
		
		cout << "PATH_VAR: " << str_paths << endl;
		
		// Parse by colon
		char delim = ':';
		stringstream path_stream(str_paths);
		string path;
		
		while ( getline(path_stream, path, delim)) {
			paths.push_back(path);
		}
		
		// Read path contents to find command
		DIR *dir;
		struct dirent *contents;
		
		for(int j = 0; j < paths.size(); ++j) {
			dir = opendir(paths.at(j).c_str());
			
			if (dir != NULL) {
				while (contents = readdir(dir)) {
					if ( (string)contents->d_name == cmd_path ) {
						cmd_path = paths.at(j) + '/' + cmd_path;
					}
				}
			}
			
		}
	}
	
	// If the cmd_path still doesn't have an absolute path, something
	// is wrong
	if (cmd_path.at(0) != '/') {
		cout << "ERROR: Command not found in path: ";
	}
	
	cout << "CMD_PATH: " << cmd_path << endl;
	
	// Convert vector to char array
	char ** cmd_array = new char*[command.size()];
	int arg_count = 0;
	
	for( int j = 0; j < command.size() + 1; ++j) {
		if (j < command.size()) {
			cmd_array[j] = new char[command[j].size() + 1];
			strcpy(cmd_array[j], command[j].c_str());
			cout << cmd_array[j] << endl;
			arg_count ++;
		}
		// The last item is null char
		else {
			cmd_array[j] = (char *) 0;
		}
	}
	
	

	
	if (command.at(command.size()-1).rfind("&") != string::npos) {
		// TODO run current job in background
		child_pid = fork();
		if (child_pid == 0)
		{
			//execv(cmd_path, args);
		}
		cout << "RUN JOB IN BACKGROUND\n";
	} else {
		// TODO run current job in foreground
		//execv(cmd_path, args);
		cout << "RUN JOB IN FORGROUND\n";
	}
	
	for (int j = 0; j < command.size() + 1; ++j) {
		delete [] cmd_array[j];
	}
	delete [] cmd_array;
	
	return;
}

int exec_cmd(char *cmd, char *args[], int arg_num, int pipe)
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
	else if ((strcmp(cmd, "cd") == 0) || (strcmp(cmd, "cd&") == 0))
	{
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

	struct job
	{
		int jobid;
		pid_t pid;
		char *command;
	};

	struct comm
	{
		char cmd[10];
		char *args[MAX_CMDS];
		int arg_num;
	};

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
		
		arg_counter = 0;
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

		while (cmd != NULL)
		{	
			cmd = (strtok(NULL, DELIMS));

			if (cmd != NULL)
			{
				comms[0].args[arg_counter] = cmd;
				comms[0].arg_num++;			
			}
						
		} //end while

		//Terminate args with NULL char
		comms[0].args[comms[0].arg_num + 1] = NULL;
		
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

		
		
		//Exit when user enters "exit" or "quit"
		if ((strcmp(comms[0].cmd, "exit") == 0) || (strcmp(comms[0].cmd, "quit") == 0))
		{
			break;
		}

		//Execute command
		exec_cmd(comms[0].cmd, comms[0].args, comms[0].arg_num, pipe_counter);
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
