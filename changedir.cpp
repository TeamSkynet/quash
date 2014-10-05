#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

int main(int argc, char *argv[])
{

	if(argc != 2) {
		string inPath;
		cout << "Please enter a valid directory path: ";
		cin >> inPath;
		chdir(inPath.c_str());
	} else { 
		chdir(argv[1]);
	}
	
	char *path=NULL;
	size_t size;
	path=getcwd(path,size);
	cout << "\nCurrent Path: " << path << endl << endl;
	return 0;
}
