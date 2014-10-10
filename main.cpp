#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include "Quash.h"

using namespace std;

int main(int argc, char **argv, char **envp)
{
	/*
  	char** env;
  	for (env = envp; *env != 0; env++)
  	{
    	char* thisEnv = *env;
    	printf("%s\n", thisEnv);    
  	}
	*/

	Quash quash(envp);
	quash.prompt();
	
	string input;
	while(getline(cin, input)) {
	
		quash.parse(input);

		quash.prompt();
	}
	
	cout << endl;
	
	return 0;
}

