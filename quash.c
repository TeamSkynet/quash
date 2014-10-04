#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LEN 256
#define DELIMS " \t\r\n"

int main(int argc, char *argv[])
{
  char *cmd;
  char ln[MAX_LEN];

  while(1) 
  {
    printf("quash> ");
    if (!fgets(ln, MAX_LEN, stdin))
    {
      break;
    }

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
    
  }

  return 0;

}
