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
    // signal(SIGCONT, handleContinueSignal);
    // signal(SIGSTOP, handlePauseProcess);
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
        }
    }
    printf("process has finished\n");
    destroyClk(false);
    kill(getppid(), SIGUSR1);
    return 0;
}

// void handleContinueSignal() {
//     int myGlobalStaticContinueVariable = 1; // Or some other handling code
// }

// void handlePauseProcess() {
//     printf("Pausing child with pid %d\n", getpid());
//     pause();
// }