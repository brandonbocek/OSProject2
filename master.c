#include "palindrome.h"

int shmid;
struct SharedMemory *shm;

// This can be adjusted if the file to read from has more lines
enum { MAXLINES = 100 };

int main(int argc, char* argv[]) {
	// Signal handler
	signal(SIGINT, signalHandler);
	int c;

	// Keeps track of how many slaves were spawned
	int totalProcesses;
	int index = 0;
	int max_writes = 5;
	int numSlaveProcesses = 19;
	int terminationTime = 60;
	char *fileToReadFrom;
	
	// Default file to read from unless the user changes it
	fileToReadFrom = "palin.in";

	// Handling Command Line arguments
	while ((c = getopt (argc, argv, "hf:w:t:")) != -1) {
		switch (c) {
			case 'f':
				fileToReadFrom = optarg;
				break;
	
			// This one seg faults
			case 'w':
				if(isdigit(*optarg))
					max_writes = atoi(optarg);
				else{
					fprintf(stderr, "Give a number with -w\n");
					exit(EXIT_FAILURE);
				}	
				break;
			case 't':
				if(isdigit(*optarg))
					terminationTime = atoi(optarg);
				else{
					fprintf(stderr, "Give a number with -t\n");
					exit(EXIT_FAILURE);
				}	
				break;
			case 'h':
				printHelpMenu();
				exit(EXIT_SUCCESS);
			case '?':
				if(optopt == 'f' || optopt == 'w' || optopt == 't') {
					fprintf(stderr, "-%c needs to have an argument!\n", optopt);
				} else {
					fprintf(stderr, "%s is an unrecognized flag\n", argv[optind - 1]);
				}
				break;
			default:
				printHelpMenu();
		}
	}

	// Read from a file into an array
	int lineNum = 0;
	char lines[MAXLINES][BUFSIZ];
	FILE *fp = fopen(fileToReadFrom, "r");
	if (fp == 0) {
		fprintf(stderr, "ERROR: Failed to open the file to read\n");
		exit(EXIT_FAILURE);
	}
	while( lineNum < MAXLINES && fgets(lines[lineNum], sizeof(lines[0]), fp)) {
		lines[lineNum][strlen(lines[lineNum])-1] = '\0';
		lineNum = lineNum + 1;
	}

	fclose(fp);

	// Create shared memory id
	if((shmid = shmget(key, sizeof(struct SharedMemory) * 2, IPC_CREAT | 0666)) < 0) {
		fprintf(stderr, "ERROR: shmget() failed.\n");
		exit(EXIT_FAILURE);
	}
	
	// Attach shared memory
    if ((shm = (struct SharedMemory *)shmat(shmid, NULL, 0)) == (struct SharedMemory *) -1) {
        	fprintf(stderr, "ERROR: shmat() failed.\n");
        	exit(EXIT_FAILURE); 
    }
	
	// Time 2 - Time 1 = Seconds that have past
	time_t t1 = time(NULL);
	time_t t2 = time(NULL); 
	
	// Process ID's
	pid_t pid, slaves[numSlaveProcesses];
	
	// Save data to shared memory
	shm->max_writes = max_writes;
	shm->numSlaveProcesses = numSlaveProcesses + 1;
	shm->turn = 0;
	shm->wordIndexToGet = -1;

	// Begin master process
	// Terminate after terminationTime elapses
	while((t2 - t1) < terminationTime) {
		if(totalProcesses < 20) {
			// Only fork the number of children specified
			if(index < numSlaveProcesses) {
				pid = fork();
				slaves[index] = pid;
				totalProcesses++;
				index++;
			}
			
			if(pid == -1) {
				fprintf(stderr, "ERROR: Fork failed.\n");
				killAllProcesses();
				exit(EXIT_FAILURE);
			
			// Spawn a slave/child process 
			} else if(pid == 0) {
				char *id;
				sprintf(id, "%i", index);

				// Indicate a new process is running (almost running)
				shm->processes++;

				// Which word to grab
				shm->wordIndexToGet++;
				
				// Creating a new process for the slave
				execl("./palin", id, lines[shm->wordIndexToGet], (char *)NULL);

				//If child program exec fails, _exit()
				//http://man7.org/linux/man-pages/man2/_exit.2.html
				_exit(EXIT_FAILURE);

			}
			
			//Reset time to know how much time has passed
			t2 = time(NULL);					
		
		// If too many processes, terminate
		} else {
			fprintf(stderr, "ERROR: Will terminate all processes, b/c there are too many.\n");
			signalHandler(2);
			exit(EXIT_FAILURE);
		}
		
		// Leave the Master if all Slaves are Dead
		int check = slavesAreStillAlive(slaves, numSlaveProcesses);
		if(check == 1) {
			break;
		}
			
	}

	// Kill all the processes if time runs out
	if((wait(NULL) > 0) && (shm->processes != 0)) {
		fprintf(stderr,"ERROR: All processes will be terminated because the Time Limit was reached.\n");
		signalHandler();
	} else {
		// Ending Successfully
		killAllProcesses();
		printf("%i Writes for %i Processes have Finished the Palindrome Sorting\n", max_writes, numSlaveProcesses);
	}
	return 0;
}
// END OF MAIN

// Release shared memory
void killAllProcesses() {
	shmctl(shmid, IPC_RMID, NULL);
	shmdt(shm);
}

// Prints the help menu on -h
void printHelpMenu() {
	printf("\n\t\t\t~~Help Menu~~\n\t\t-h This Help Menu Printed\n");
	printf("\t\t-f *fileName* The file to read from is changed\n");
	printf("\t\t-w *NUM* Modify the number of max_writes for each child is set\n");
	printf("\t\t-t *NUM* Modify the number of seconds before auto termination\n");
}

// Kills all when signal is received
void signalHandler() {
    	pid_t id = getpgrp();
	printf("Signal was Received and the Master Process with ID: %i will be terminated\n", id);
	killAllProcesses();
    	killpg(id, SIGINT);
    	exit(EXIT_SUCCESS);
}

// Returns 1 (true) if any slaves are still running
int slavesAreStillAlive(pid_t slaves[], int size) {
	int i, status;
	
	for(i = 0; i < size; i++) {
		pid_t wid;
		// return immediately if no child has exited
		wid = waitpid(slaves[i], &status, WNOHANG);
		
		if(wid != 0)
			slaves[i] = 0;
	}
	
	for(i = 0; i < size; i++) {
		if(slaves[i] == 0)
			continue;
		else
			return 0;
	}	
		return 1;
}
