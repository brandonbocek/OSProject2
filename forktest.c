#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(void) {
	
	char singleLine[150];
	FILE * fPointer;
	fPointer = fopen("me.txt", "r");
	
	if (fork() == -1) {
		perror("Failed to fork");
		return 1;
	}
	fgets(singleLine, 150, fPointer);
	printf("%s", singleLine);
	fclose(fPointer);
	singleLine[150] = NULL;
//	printf("Process %ld got %c\n", (long)getpid(), c);
	return 0;
}
