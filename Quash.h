#ifndef QUASH
#define QUASH

#include <iostream>
#include <string>
#include <vector>


#define READ_END 0
#define WRITE_END 1

using namespace std;

class Quash
{
	public:
		enum JobMode {
			FOREGROUND,
			BACKGROUND
		};
		
		struct Job {
			pid_t pid;
			string command;
		};
		
		Quash(char **envp);
		~Quash(void);
		
		void execute(vector<string> commands);
		void parse(string input);
		void prompt(void);
		
	private:
		char *cur_dir;
		char **envp;
		pid_t init_pid;
		pid_t init_pgid;
		vector<Job> jobs;
		
		void exec_set(string key, string value);
		void exec_cd(string path);
		void exec_exit(void);
		void exec_list(void);
		void exec_kill(Job job);
		void update_jobs(void);

		bool builtInCmd(vector<string> command);
		
		void exec_job(const char *cmd_path, char **command, string command_str, char mode, int pipes[], int commands_i, int commands_size);
};

#endif

