#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();
    int rem = atoi(argv[1]); //remaining time upone dequeuing from ready queue
    //TODO The process needs to get the remaining time from somewhere
    int currtime;
    int prevtime = getClk();
    while (rem> 0){
        currtime = getClk();
        if(prevtime != currtime){
            rem = rem - 1;
            prevtime = getClk();
        }
    }
    printf("process is finished\n");
    destroyClk(false);
    kill(getppid(), SIGUSR1);
    return 0;
}
