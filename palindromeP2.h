
#ifndef palindromeP2_H

#define palindromeP2_H
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>

void killAllProcesses();
void handleSignals();
void helpDisplay();

enum state {
	idle,
	wantsIn,
	inCritical
};

struct SharedMemory {

	int flags[19];
	int turn;
	int maxWrites;
	int numSlaveProcess;
	int numProcesses;
};

int checkSlaves(pid_t [], int, int);

key_t key = 0;

#endif
