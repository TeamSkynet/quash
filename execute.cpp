#include <iostream>
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

void exec_set(string envvar, string val)
{
	if (setenv(envvar.c_str(), val.c_str(), 1) != 0) {
		cout << "setenv error, exit code not zero";
	}
}


void execute(vector<string> commands)
{
	
	for(int i = 0; i < commands.size(); ++i) {
	
		// Parse command by space
		vector<string> command;
		char command_delim = ' ';
		stringstream cmd_stream(commands.at(i));
		string current_cmd;
		
		while ( getline(cmd_stream, current_cmd, command_delim) ) {
			command.push_back(current_cmd);
		}
		
		if(command.at(0) == "cd") {
			string path = "";
			
			// Create new path
			for (int j = 1; j < command.size(); ++j) {
				path += command.at(j) + " ";
			}
	
			chdir(path.c_str());
			
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
			cout << "Command not found in path: ";
		}
		
		cout << "CMD_PATH: " << cmd_path << endl;
		
		// Convert vector to char array
		char ** cmd_array = new char*[command.size()];
		
		for( int j = 0; j < command.size() + 1; ++j) {
			if (j < command.size()) {
				cmd_array[j] = new char[command[j].size() + 1];
				strcpy(cmd_array[j], command[j].c_str());
				cout << cmd_array[j] << endl;
			}
			// The last item is null char
			else {
				cmd_array[j] = (char *) 0;
			}
		}
		
		if (commands.at(i).rfind("&") != string::npos) {
			// TODO run current job in background
			cout << "RUN JOB IN BACKGROUND\n";
		} else {
			// TODO run current job in foregroune
			cout << "RUN JOB IN foregroune\n";
		}
		
		for (int j = 0; j < command.size() + 1; ++j) {
			delete [] cmd_array[j];
		}
		delete [] cmd_array;
		
	}
	
	
	
	return;
}

int main()
{
	vector<string> command;
	command.push_back("ls");
	execute(command);
	
	return 0;
}
