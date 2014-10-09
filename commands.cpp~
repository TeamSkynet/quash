#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>

#include <dirent.h>

#include <sys/param.h>
#include <unistd.h>

using namespace std;

struct job {
	pid_t pid;
	pid_t pgrp;
	string name;
	string path;
	bool foreground;
}

vector<job> jobs;

char* current_dir;
char** envp;

pid_t current_pid();
pid_t current_pgrp();

void init_quash(char **envp)
{
	current_dir = new char[MAXPATHLEN];
	this->envp = envp;
	current_pid = getpid();
	current_pgrp = getpgrp();
}

	
	

void exec_set(string envvar; string val)
{
	if (setenv(envvar.c_str(), val.c_str(), 1) != 0) {
		"setenv error, exit code not zero";
	}
}

void exec_list()
{
	for(int i = 0; i < jobs.size(); ++i) {
		cout << "[" << i << "] " << jobs.at(i).pid << " " jobs.at(i) << jobs.at(i).title << endl;
	}
}


void execute(  )
{
	
	for(int i = 0; i < commands.size(); ++i) {
	
		// Parse command by space
		vector<string> command;
		char command_delim = ' ';
		stringstream cmd_stream(commands.at(i));
		string current_cmd;
		
		while ( getline(cmd_stream, current_cmd, delim) {
			command.push_back(current_cmd);
		}
		
		// Run built in commands
		if (builtInCmd(command)) {
		
			if(command.at(0) == "cd") {
				string path = "";
				
				// Create new path
				for (int i = 1; i < command.size(), ++i) {
					path += command.at(i) + " ";
				}
		
				chdir(path.c_str());
				
				return;
			}
			else if(command.at(0) == "set") {
				for (int i = 1; i < command.size(), ++i) {
					path += command.at(i);
					
					if (i < command.size()-1)
						path += " ";
				}
				
				exec_set(path.substr(0, path.find("=")), path.substr(path.find("=")+1));
				
				return;
			}
		
		
			return;
		}
		
		
		
		string cmd_path(command.at(0));
		
		
		// If command is not specified as absolute path, search directories in 
		// PATH environment variable
		if(cmd_path.at(0) != '/') {
		
			// Parse PATH into array, each index a directory
			vector<string> paths;
			
			char* path_var = getenv("PATH");
			
			if (path_var != NULL) {
				string path_var_str = path_var;
			} else {
				path_var_str = "";
			}
			
			cout << "PATH_VAR: " << path_var_str << endl;
			
			// Parse by colon
			char delim = ':';
			stringstream path_stream(path_var_str);
			string path;
			
			while ( getline(path_stream, path, delim)) {
				paths.push_back(path);
			}
			
			// Read path contents to find command
			DIR *dir;
			struct dirent *path_contents;
			
			for(int = 0; i < paths.size(); ++i) {
				dir = opendir(paths.at(i).c_str());
				
				if (dir != NULL) {
					while (contents = readdir(dir)) {
						if ( (string)contents->d_name == cmd_path ) {
							cmd_path = path.at(i) + '/' + cmd_path;
						}
					}
				}
				
			}
		}
		
		// If the cmd_path still doesn't have an absolute path, something
		// is wrong
		if (cmd_path.at(0) != '/') {
			cout << "Command not found in path: ";
		}
		
		cout << "CMD_PATH: " << cmd_path << endl;
		
		// Convert vector to char array
		char** cmd_array = new char*[command.size()];
		
		for( int i = 0; i < command.size() + 1; ++i) {
			if (i < command.size()) {
				cmd_array[i] = new char[command[i].size() + 1];
				strcpy(cmd_array[i], command[i].c_str());
				cout << command_array[i] << endl;
			}
			// The last item is null char
			else {
				command_array[i] = (char *) 0;
			}
		}
		
		if (commands.at(i).rfind("&") != string::npos) {
			// TODO run current job in background
			cout << "RUN JOB IN BACKGROUND\n";
		} else {
			// TODO run current job in foregroune
			cout << "RUN JOB IN foregroune\n";
		}
		
		for (int i = 0; i < command.size() + 1; ++i) {
			delete [] cmd_array[i];
		}
		delete [] cmd_array;
		
	}
	
	
	
	return;
}

