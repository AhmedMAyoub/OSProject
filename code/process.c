#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();
    int start = atoi(argv[1]); //start time upon dequeuing from ready queue
    int rem = atoi(argv[2]); //remaining time upone dequeuing from ready queue
    int finish = atoi(argv[3]); //finish time 
    int id = atoi(argv[4]); //id of proces
    //TODO The process needs to get the remaining time from somewhere
    int currtime;
    printf("process with pid %d and start time %d and finish time %d and remaining time %d\n", id, start, finish, rem);
    while (rem> 0){
        currtime = getClk();
        rem = finish - currtime;
    }
    printf("process is finished\n");
    destroyClk(false);
    kill(getppid(), SIGUSR1);
    return 0;
}
