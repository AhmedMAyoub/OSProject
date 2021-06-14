#include "headers.h"

int algoChoice = 0;
int quantum = 0;
int processCount = 0;

int main(int argc, char *argv[])
{
    initClk();
    struct msgBuffProcesses processToReceive;
    algoChoice = atoi(argv[1]);
    quantum = atoi(argv[2]);
    processCount = atoi(argv[3]);
    printf("gowa scheduler algo choice is %d\n", algoChoice);
    printf("gowa scheduler quantum is %d\n", quantum);
    printf("gowa scheduler pCount is %d\n", processCount);
    key_t msgQScheduler;
    int rec_val, msgQSched_id;
    msgQScheduler = ftok("keyfile.txt", 33); // create unique key
    msgQSched_id = msgget(msgQScheduler, 0666 | IPC_CREAT);
    if (msgQSched_id == -1)
    {
        perror("Error in create");
    }
    else
    {
        printf("Message Queue inside sched ID = %d\n", msgQSched_id);
    }
    int receivedP = 0;
    processToReceive.processtype=1;
    while (getClk() != 100)
    {
        printf("time is %d\n", getClk());
        while (receivedP < processCount)
        {
            rec_val = msgrcv(msgQSched_id, &processToReceive, sizeof(processToReceive.p), 0, !IPC_NOWAIT);
            if (rec_val == -1)
            {
                perror("Error in receive");
            }
            else
            {
                printf("new process received with processId %d", processToReceive.p.id);
                receivedP++;
            }
        }
    }
    destroyClk(true);
    exit(0);
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
}