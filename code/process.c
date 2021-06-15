#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

void handleContinueSignal();
void handlePauseProcess();
void *shmaddr1;
int shmid;

int main(int agrc, char *argv[])
{
    initClk();
    signal(SIGCONT, handleContinueSignal);
    signal(SIGSTOP, handlePauseProcess);
    // shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0644);
    // if (shmid == -1)
    // {
    //     perror("Error in create");
    //     exit(-1);
    // }
    // else
    // {
    //     printf("\nShared memory ID = %d\n", shmid);
    // }
    int start = atoi(argv[1]); //start time upon dequeuing from ready queue
    remainingtime = atoi(argv[2]); //remainingtimeaining time upone dequeuing from ready queue
    int id = atoi(argv[3]); //id of proces
    //TODO The process needs to get the remainingtimeaining time from somewhere
    int currtime;
    printf("process with pid %d and start time %d and remainingtimeaining time %d\n", id, start,remainingtime);
    int prevtime = getClk();
    while (remainingtime> 0){
        currtime = getClk();
        if(prevtime != currtime){
            remainingtime = remainingtime - 1;
            prevtime = getClk();
            printf("process inside process with pid %d and start time %d and remainingtimeaining time %d\n", id, start,remainingtime);
        }
    }
    printf("process is finished\n");
    destroyClk(false);
    kill(getppid(), SIGUSR1);
    return 0;
}

void handleContinueSignal() {
    int myGlobalStaticContinueVariable = 1; // Or some other handling code
}

void handlePauseProcess() {
    // shmaddr1 = shmat(shmid, (void *)0, 0);
    // if (shmaddr1 == -1)
    // {
    //     perror("Error in attach in Process");
    //     exit(-1);
    // }
    // else
    // {
    //     printf("\nProcess: Shared memory attached at address %x\n", shmaddr1);
    //     printf("\nProcess: Sending remaining time before pause %x\n", remainingtime);
    //     strcpy((int *)shmaddr1, remainingtime);
    // }
    printf("Pausing child with pid %d\n", getpid());
    pause();
}