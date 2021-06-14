#include "queue.h"
#include <string.h>

int algoChoice = 0;
int quantum = 0;
int processCount = 0;
int currTime = 0;
int prevTime = 0;
bool isFinished = true;
int totalProcessesDone = 0;

void FCFS(struct process *CurrProcess)
{
    isFinished = false;
    int Processpid = fork();
    if (Processpid == 0)
    {
        Processpid = getpid();
        int CurrClock = getClk(); //get current time when process enters
        printf("Current time upon entering %d\n", CurrClock);
        strcpy(CurrProcess->status, "running");            //setting status of process to running
        int FinishTime = CurrClock + CurrProcess->runTime; //calculating when process should be finished
        printf("current process is %d and finish time is %d\n", CurrProcess->id, FinishTime);
        CurrProcess->startTime = getClk(); //set start time for process
        CurrProcess->finishTime = FinishTime;
        CurrProcess->remainingTime = CurrProcess->runTime;
        char start[20];
        sprintf(start, "%d", CurrProcess->startTime);
        char remaining[20];
        sprintf(remaining, "%d", CurrProcess->remainingTime);
        char finish[20];
        sprintf(finish, "%d", CurrProcess->finishTime);
        char id[20];
        sprintf(id, "%d", CurrProcess->id);
        char *args[] = {"./process.out", start, remaining, finish, id, NULL};
        printf("initializing process with id %d\n", CurrProcess->id);
        execvp(args[0], args);
    }
    else
    {

        return;
    }
}

void ReceivingLoopFCFS()
{
    struct Queue *readyQueue = Queue_Constructor();
    struct process *processesArr = (struct process *)malloc(processCount * sizeof(struct process));
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
    while (1)
    {
        currTime = getClk();
        if (currTime != prevTime)
        {
            if (receivedP == processCount) //will be changed once we implement algorithms
            {
                break;
            }
            printf("time is %d\n", currTime);
            prevTime = currTime;
            while (1)
            {
                struct process *p;
                struct msgBuffProcesses processToReceive;
                processToReceive.processtype = 1;
                rec_val = msgrcv(msgQSched_id, &processToReceive, sizeof(processToReceive.p), 0, !IPC_NOWAIT);
                int printTime = getClk();
                printf("processRec with id %d received at %d\n", processToReceive.p.id, printTime);
                if (processToReceive.p.id == -1)
                {
                    break;
                }
                else
                {
                    processesArr[receivedP] = processToReceive.p;
                    enqueue(readyQueue, &processesArr[receivedP]);
                    receivedP++;
                }
            }
        }
        if (!isEmpty(readyQueue) && isFinished)
        {
            FCFS(dequeue(readyQueue));
        }
    }
    while (totalProcessesDone != processCount)
    {
        if (isFinished && !(isEmpty(readyQueue)))
        {
            //printf("total %d\n", totalProcessesDone);
            FCFS(dequeue(readyQueue));
            //print_Queue(readyQueue);
        }
    }
}

//First come first serve algorithm

void handleProcessFinish()
{
    printf("inside handler\n");
    isFinished = true;
    totalProcessesDone = totalProcessesDone + 1;
}

int main(int argc, char *argv[])
{
    initClk();
    signal(SIGUSR1, handleProcessFinish);
    algoChoice = atoi(argv[1]);
    quantum = atoi(argv[2]);
    processCount = atoi(argv[3]);
    printf("gowa scheduler algo choice is %d\n", algoChoice);
    printf("gowa scheduler quantum is %d\n", quantum);
    printf("gowa scheduler pCount is %d\n", processCount);
    switch(algoChoice){
        case 1:
            ReceivingLoopFCFS();
            break;
    }
    printf("rec Done scheduler Algorithm 1 \n");
    destroyClk(true);
    exit(0);
    raise(SIGKILL);
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
}