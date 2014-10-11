#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <algorithm>
#include <fcntl.h>

#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>

#include "Quash.h"


using namespace std;

Quash::Quash(char **envp)
{
	cur_dir = new char[MAXPATHLEN];	
	this->envp = envp;
	init_pid = getpid();
	init_pgid = getpgrp();

}


Quash::~Quash(void)
{
	delete cur_dir;
}


void Quash::exec_set(string key, string value)
{
	// Set the environment variable with key='key' to value 'value'
	if(setenv(key.c_str(), value.c_str(), 1) != 0)
	{
		cout << "ERROR SETTING VARIABLE\n";
	}
}


void Quash::exec_cd(string path)
{
	// If no path specified, chdir into the HOME directory
	if(path.size() == 0) {
		chdir(getenv("HOME"));
	}
	else if(chdir(path.c_str()) == -1) {
		cout << path << ": Not found";
	}
}


void Quash::exec_exit(void)
{
	// Exit the program.
	exit(EXIT_SUCCESS);
}


void Quash::exec_kill(Job job)
{
	if(kill(job.pid, SIGKILL) == 0) {
		cout << "[" << job.pid << "] " << job.pid << " killed " << job.command << endl;
	}
	else {
		cout << job.pid << ": could not be killed";
	}
}


void Quash::update_jobs(void)
{
	// Loop through each job and remove jobs that are no longer running
	vector<Job>::iterator it = jobs.begin();
	while(it != jobs.end()) {
		// If kill(pid, signal=0) doesn't succeed (!=0) then the pid doesn't exist
		if(kill((*it).pid, 0) != 0) {
			cout << "[" << ((*it).pid + 200) << "] " << (*it).pid << " finished " << (*it).command << endl;
			it = jobs.erase(it);
		}
	}

}


void Quash::exec_list(void)
{
	// Loop through each still-running job and show its info
	vector<Job>::iterator it = jobs.begin();
	while(it != jobs.end()) {
		// If kill(pid, signal=0) does succeed (==0) then the pid does exist
		if(kill((*it).pid, 0) == 0) {
			cout << "[" << ((*it).pid + 200) << "] " << (*it).pid << " " << (*it).command << endl;
		}
	}
}


void Quash::execute(vector<string> commands, bool r_in, bool r_out, string fname)
{
	// Show a message to the use for which bg jobs have completed
	update_jobs();
	
	int pipesfd[2];
	pipe(pipesfd);
	
	// NOTE: if commands.size > 1 then we need to pipe between the commands
	for(int i = 0 ; i < commands.size() ; ++i) {
		// Note if it should be executed in the foreground or background
		char mode = 'f';
		if(commands.at(i).rfind("&") != string::npos) {
			mode = 'b';
			// Remove the & character
			commands.at(i) = commands.at(i).replace(commands.at(i).rfind("&"), 1, " ");
		}
		else {
			mode = 'f';
		}
		
		//printf("mode after mode = %s\n", mode);
		// Replace ALL double spaces with a single space
		while(commands.at(i).find("  ") != string::npos) {
			commands.at(i) = commands.at(i).replace(commands.at(i).find("  "), 2, " ");
		}
		
		// Remove spaces from the beginning of the command
		while(commands.at(i).find(" ") == 0) {
			commands.at(i) = commands.at(i).substr(1);
		}

		// Split the command into chunks by a <space> character
		vector<string> command;
		
		char delim = ' ';
		stringstream cmd_stream(commands.at(i));
		string current_cmd;
		while(getline(cmd_stream, current_cmd, delim)) {
			command.push_back(current_cmd);
		}
		
		// Try to run the command if it is a built-in command
		if(builtInCmd(command)) {
			return;
		}
		
		string cmd_path(command.at(0));
		// If executable is not specified in the absolute path format, then 
		// search through the directories in the environment variable PATH
		if(cmd_path.at(0) != '/') {
		
			// Split the PATH env var into an array of paths
			vector<string> paths;
			
			const char* path_env = getenv("PATH");
			string str_paths = path_env?path_env:"";

			char delim = ':';
			stringstream path_stream(str_paths);
			string path;
			while(getline(path_stream, path, delim)) {
				paths.push_back(path);
			}
			
			
			// Check the paths for the executable
			DIR *dir;
			struct dirent *contents;
			for(int j = 0 ; j < paths.size() ; ++j) {
				dir = opendir(paths.at(j).c_str());
				if(dir != NULL) {
					while(contents = readdir(dir)) {
						if((string)contents->d_name == cmd_path) {
							cmd_path = paths.at(j) + '/' + cmd_path;
						}
					}
				}
			}
			
		}
		
		
		// If the cmd_path still doesn't have an absolute path, something
		// is wrong
		if(cmd_path.at(0) != '/')
		{
			cout << cmd_path << ": not found in path";
		}
		
		// Convert vector<string> to char* array
		char ** command_arr = new char*[command.size()+1];
		for(int j = 0 ; j < command.size()+1 ; ++j) {
			if(j < command.size()) {
				command_arr[j] = new char[command[j].size() + 1]; 
				strcpy(command_arr[j], command[j].c_str());
			}
			// Last item in arr needs to be a null char
			else {
				command_arr[j] = (char *) 0;
			}
		}
		
		
		// Execute command
		exec_job(cmd_path.c_str(), command_arr, commands.at(i), mode, pipesfd, 
				(int)i, (int)commands.size(), r_in, r_out, fname);
		
	
		// Delete the array we created
		for(int j = 0 ; j < command.size() ; ++j) {
			delete [] command_arr[j];
		}
		delete [] command_arr;
		

		// Delay for background STDOUT race condition
		usleep(20000);
	}
	
	return;
}


void Quash::exec_job(const char *cmd_path, char **command, string command_str, 
				char mode, int pipes[], int commands_i, int commands_size, bool r_in, bool r_out, string fname)
{
	int status = 0;
	
	pid_t pid;
	pid = fork();
	int fd;

	//cout << "in exec_job:" << r_in << " " << r_out << "\n";
	
	if(pid == -1) {
		cout << cmd_path << ": fork error" << endl;
		exit(EXIT_FAILURE);
	}
	else if(pid == 0) {
		
		if(commands_size == 2 && commands_i == 0) {
			close(pipes[0]);
			dup2(pipes[1], 1);
			close(pipes[1]);
		}
		else if(commands_size == 2 && commands_i == 1) {
			close(pipes[1]);
			dup2(pipes[0], 0);
			close(pipes[0]);

			if (r_in)  //redirect input
			{
				fd = open(fname.c_str(), O_RDONLY);
				dup2(fd, 0);		
			}			
			else if (r_out)	{
				fd = open(fname.c_str(), O_WRONLY | O_CREAT,777);
				dup2(fd, 1);
			}
			
		}
		else if (commands_size == 1 && commands_i == 0 && r_in) {
			//cout << "commands 1, r_in\n";
			fd = open(fname.c_str(), O_RDONLY);
			dup2(fd, 0);
		}		
		else if (commands_size == 1 && commands_i == 0 && r_out) {
			//cout << "commands 1, r_out\n";
			fd = open(fname.c_str(), O_WRONLY | O_CREAT,777);
			dup2(fd, 1);
		}
		close(fd);
					
		if(mode == 'f') {
			// Execute the command and exit success
			execve(cmd_path, command, envp);
			exit(EXIT_FAILURE);
		}
		else if(mode == 'b') {
			// Show the user the job is running in the background with the 
			// format: [JOBID] PID running in background
			cout << "[" << getpid() + 200 << "] " << getpid() << 
				" running in background" << endl;
			
			pid = fork();

			if(pid == -1) {
				cout << cmd_path << ": fork2 error" << endl;
				_exit(EXIT_FAILURE);
			}
			else if(pid == 0) {
				// Show the user the job is running in the background with the 
				// format: [JOBID] PID running in background
				cout << "[" << getpid() + 200 << "] " << getpid() << 
					" running in background" << endl;
				
				// Execute the command and exit success
				execve(cmd_path, command, envp);
				_exit(EXIT_FAILURE);
			}
			else {
				_exit(EXIT_SUCCESS);
			}
			
		}
	}
	else {
		if(mode == 'f') {
			// Wait for the job to complete
			wait(&status);

		}
		else if(mode == 'b') {
			// Wait for the the child; won't block for long since it is givin 
			// it is its own leader
			waitpid(pid, &status, 0);
			
			// Add the job to the array
			Job job;
			job.pid = pid;
			job.command = command_str;
			jobs.push_back(job);
		}
		
	}
}


bool Quash::builtInCmd(vector<string> command)
{
	if(command.at(0) == "cd") {
		// (Re-)build the path
		string path = "";
		for(int i=1 ; i<command.size() ; ++i) {
			path += command.at(i);
			
			if(i < command.size()-1) path += " ";
		}
		
		exec_cd(path);
		return true;
	}
	else if(command.at(0) == "exit" || command.at(0) == "quit") {
		exec_exit();
		return true;
	}
	else if(command.at(0) == "jobs") {
		exec_list();
		return true;
	}
	else if(command.at(0) == "kill") {
		for(int i=1 ; i<jobs.size() ; ++i) {
			stringstream ss;
			ss << jobs.at(i).pid;
			int jid;
			ss >> jid;
			jid += 200;

			ss << command.at(1);
			int rjid;
			ss >> rjid;
			if(jid == rjid) {
				exec_kill(jobs.at(i));
				return true;
			}
		}
		
		cout << command.at(1) << ": invalid job id" << endl;
	}
	else if(command.at(0) == "set") {
		// (Re-)build the path
		string path = "";
		for(int i=1 ; i<command.size() ; ++i) {
			path += command.at(i);
			
			if(i < command.size()-1) path += " ";
		}
		
		// Split the key and value by the '=' sign
		exec_set(path.substr(0, path.find("=")), path.substr(path.find("=")+1));

		return true;
	}
	
	return false;
}


void Quash::parse(string input)
{
	// Split the input into an array of commands
	vector<string> commands;
	
	char delim = '|';
	stringstream in_stream(input);
	string cmd;
	string fname = "none";
	string redir_out = ">";
	string redir_in = "<";
	size_t found_redir_out = cmd.find(redir_out);
	size_t found_redir_in = cmd.find(redir_in);
	bool r_out = false;
	bool r_in = false;

	while(getline(in_stream, cmd, delim)) {
		
		//check for redirection operator
		cout << "cmd = " << cmd << "\n";
		found_redir_out = cmd.find(redir_out);
		found_redir_in = cmd.find(redir_in);

		if (found_redir_out != std::string::npos)
		{
			r_out = true;
			fname = cmd.substr(cmd.find(redir_out) + 1); 
			cmd = cmd.substr(0, cmd.find(redir_out));
		}
		else if (found_redir_in != std::string::npos)
		{
			r_in = true;
			fname = cmd.substr(cmd.find(redir_in) + 1); 
			cmd = cmd.substr(0, cmd.find(redir_in));
		}
		fname.erase(std::remove(fname.begin(),fname.end(),' '),fname.end());  //Removes spaces
		cout << "cmd = " << cmd << "\n";
		cout << "fname = " << fname << "\n";

		commands.push_back(cmd);
	}

	execute(commands, r_in, r_out, fname);

}


void Quash::prompt(void)
{
	// Update cur_dir with the current working directory
	if(!getcwd(cur_dir, MAXPATHLEN)) 
	{
		cout << "ERROR: Could not getcwd()";
	}
	
	// Output the prompt line
	cout << "[" << getenv("LOGNAME") << " " << cur_dir  << "]> ";
}

