#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

	char *args[] = {"Hello", "All", NULL};
	printf("About to run a new program\n");
	execv("./np", args);
	perror("execv"); /* if I get here, execv failed */
	printf("End of main program");
	return 0;
}
