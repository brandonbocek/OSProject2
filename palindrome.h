#ifndef PALINDROME_H
#define PALINDROME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>


enum state {idle, want_in, in_cs};
key_t key = 1221;

int slavesAreStillAlive(pid_t [], int);
int isPalindrome(char []);
void signalHandler();
void killAllProcesses();
void printHelpMenu();

struct SharedMemory {

	int wordIndexToGet;
	int turn;
	int flag[19];
	int max_writes;
	int numSlaveProcesses;
	int processes;
};

#endif
