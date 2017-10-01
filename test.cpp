#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

using namespace std;

int main() {

	char* args[3];
	string ls = "ls";
	string dash_l = "-l";
	args[1] = (char*) dash_l.c_str();
	args[0] = (char*)ls.c_str();
	args[2] = NULL;
	pid_t pid = fork();
	if(pid == -1){
		perror("fork");
	}
	if(pid == 0) {
		//child
		cout << "child:" << pid << endl;
		if(execvp(args[0], args) == -1) {
			perror("exec");
		}
	}
	if(pid > 0){
		//parent
		wait(0);
		cout << "parent:" << pid << endl;
	}
	return 0;
}
