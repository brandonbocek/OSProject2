#include "palindromeP2.h"

struct SharedMemory *shm;
int shmid;

void killAllProcesses() {
	shmctl(shmid, IPC_RMID, NULL);
	shmdt(shm);
}

void signalHandler() {
	// get the process's id
	pid_t id = getpid();
	killAllProcesses();
	// kills the signal sig to the process group
	killpg(id, SIGINT);
	exit(EXIT_SUCCESS);
}

// returns 0 if a child process does not have a 0 id
int checkSlaves(pid_t slaves[], int size, int max_writes){
	int status, i;
	pid_t waitID;
	for(i=0; i<size; i++){
		waitID = waitpid(slaves[i], &status, WNOHANG);
		
		if(waitID != 0)
			slaves[i] = 0;
	}
	
	for(i=0; i<size; i++){
		if(slaves[i] == 0)
			continue;
		else
			return 0;
	}
		return 1;
}

void createAndAttachSharedMemory(){

}

int main(){

	//https://www.tutorialspoint.com/c_standard_library/c_function_signal.htm
	//sets a function to handle the signals
	signal(SIGINT, signalHandler);	

	if((shmid=shmget(key, sizeof(struct SharedMemory)*2, IPC_CREAT | 0666))<0){
		perror("shmget failed to create a shared memory id");
		exit(EXIT_FAILURE);
	}

return 0;
} 
