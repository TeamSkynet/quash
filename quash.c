#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_LEN 256
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
}


int main(int argc, char *argv[])
{
	char *cmd;  //command and argument buffer
	char *current_cmd;  //current command for execution
	char *args[MAX_LEN];  //argument array
	int arg_counter;
	char ln[MAX_LEN];

	while(1) 
	{
    	printf("quash> ");
    	if (!fgets(ln, MAX_LEN, stdin))
    	{
      		break;
    	}
		
		//Read in command and arguments
		printf("echo: %s\n", ln);
		cmd = strtok(ln, DELIMS);
		current_cmd = cmd;
		arg_counter = 0;
		while (cmd != NULL)
		{	
			cmd = (strtok(NULL, DELIMS));
			if (cmd != NULL)
			{
				args[arg_counter] = cmd;
				arg_counter++;		
			}			
		}
		printf("Command echo: %s\n", current_cmd);
		for (int i=0; i < arg_counter; i++)
		{
			printf("Arg echo: %s\n", args[i]);
		}
	}

/*			
    if ((cmd = strtok(ln, DELIMS)))
    {
      errno = 0; //Clear any errors

      //Command structure
      //TODO

      if (strcmp(cmd, "exit") != 0)
      {
        printf("You entered %s\n", cmd);
      }
      else
      {
        break;
      }

      if (errno)
      {
        perror("Command failed\n)");
      }

    }
*/    

  return 0;

} 


