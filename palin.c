#include "palindrome.h"

int shmid;
struct SharedMemory *shm;

int main(int argc, char* argv[]) {
	// Signal Handler
	signal(SIGINT, signalHandler);	
	
	// Get passed arguments from execl()
	int i = atoi(argv[0]);	
	char *word = argv[1];
 	
	// Get shared memory id that was created by the master process	
	if((shmid = shmget(key, sizeof(struct SharedMemory) * 2, 0666)) < 0) {
		fprintf(stderr, "CHILD ERROR: shmget() failed.\n");
		exit(EXIT_FAILURE);
	}
	
	// Attach the shared memory to the slave process
    if ((shm = (struct SharedMemory *)shmat(shmid, NULL, 0)) == (struct SharedMemory *) -1) {
        	fprintf(stderr, "CHILD ERROR: shmat() failed\n");
        	exit(EXIT_FAILURE);
    }
	
	// Get the time https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
	char *t = malloc(10);
	time_t theTime;
	theTime = time(NULL);
	struct tm *p = localtime(&theTime);
	strftime(t, 10, "%H:%M:%S", p);
	
	// Alerting the user on what the child process is trying to do
	fprintf(stderr, "Child PID: %i is Executing Code to Try and Enter Critical Section at Time %s\n", getpid(), t);

	// Multiple Process Solution
	// Execute code to enter critical section
	int j, c, n = shm->numSlaveProcesses;
	for(c = 0; c < shm->max_writes; c++) {
		do {
			//Raise My flag
			shm->flag[i] = want_in;
			
			//Set local variable
			j = shm->turn; 
			
			while(j != i) {
				j = (shm->flag[j] != idle) ? shm->turn : (j + 1) % n;
			}
			
			// Declare intention to enter critical section
			shm->flag[i] = in_cs;
			
			// Check that no one else is in critical section
			for(j = 0; j < n; j++) {
				if((j != i) && (shm->flag[j] == in_cs)) {
					break;
				}
			}
		} while((j < n) || ((shm->turn != i) && (shm->flag[shm->turn] != idle)));
		
		// Assign to self and enter critical section
		shm->turn = i; 
		
		/////// Critical Section Starts ////////

		// Get the time again for C_S entry
		theTime = time(NULL);
		p = localtime(&theTime);
		strftime(t, 10, "%H:%M:%S", p);
		fprintf(stderr, "\tChild PID: %i is Entering C_S at Time: %s\n", getpid(), t);
				
		// Writing a palindrome and not palindrome to correct file
		FILE *fPointer;
		if(isPalindrome(word)) {
			fPointer = fopen("palin.out", "a");

			// Exit if the file failed to open
			if(fPointer == NULL) {
				fprintf(stderr, "File failed to open\n");
				killAllProcesses();
				exit(EXIT_FAILURE);
			}
		} else {
			fPointer = fopen("nopalin.out", "a");
				
			// Exit if the file failed to open
			if(fPointer == NULL) {
				fprintf(stderr, "File failed to open\n");
				killAllProcesses();
				exit(EXIT_FAILURE);
			}
		}
		
		// Code to have a unique random number for each slave process
		// necessary because the children all spawn at the same time
		struct timespec tm;
		clock_gettime(CLOCK_MONOTONIC, &tm);
		srand((unsigned)(tm.tv_sec ^ tm.tv_nsec ^ (tm.tv_nsec >> 31)));

		// Sleep 0-2 seconds before writing
		sleep((rand() % 3));
		
		// Reset the time
		theTime = time(NULL);
		p = localtime(&theTime);
		strftime(t, 10, "%H:%M:%S", p);
		
		// Actually Prints the Potential Palindrome to a File		
		fprintf(fPointer, "PID: %i | Index: %d | String: %s\n", getpid(), i,  word);
		
		// Updating the user
		fprintf(stderr, "\t\tProcess %i at time %s wrote this word to a file:  %s\n", i, t, word);
		fclose(fPointer);
		
		// Sleep 0-2 seconds after writing
		sleep((rand() % 3));
		
		///////////// Critical Section Ends ///////////////
		j = (shm->turn + 1) % n;
		while(shm->flag[j] == idle) {
			j = (j + 1) % n;
		}
		
		// Reset the time again and print the exit message
		theTime = time(NULL);
		p = localtime(&theTime);
		strftime(t, 10, "%H:%M:%S", p);
		fprintf(stderr, "\tChild PID: %i is Exiting C_S at Time: %s\n\n", getpid(), t);
		
		// Assign turn to next waiting process; change own flag to idle
		shm->turn = j;
		shm->flag[i] = idle;
	}
	
	// Some error occured, clear shared memory and exit
	shm->processes--;
	killAllProcesses();
	exit(EXIT_FAILURE);
}

// Kills all when signal is received
void signalHandler() {
    	pid_t id = getpid();
	printf("Signal was received, slave process with PID: %i will be terminated\n", id);
	killAllProcesses();
    	killpg(id, SIGINT);
    	exit(EXIT_SUCCESS);
}

// Release shared memory
void killAllProcesses() {
	shmdt(shm);
}

//Checks for Palindrome, returns 1(true) if it is
int isPalindrome(char s[]) {
	int l, i;
	l = strlen(s);
	for( i = 0; i < l/2; i++ ){
		if(s[i] != s[l-1-i])
			return 0;
	}
	return 1;
}
