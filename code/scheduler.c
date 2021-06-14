#include "queue.h"

int algoChoice = 0;
int quantum = 0;
int processCount = 0;
int currTime = 0;
int prevTime = 0;
key_t msgQScheduler;
int rec_val, msgQSched_id;

void runHPF();
// void FCFS(struct process* CurrProcess){
//     int CurrClock = getClk(); //get current time when process enters
//     printf("Curren time upon entering %d\n", CurrClock);
//     strcpy(CurrProcess->status, "running"); //setting status of process to running
//     int FinishTime = CurrClock + CurrProcess->runTime; //calculating when process should be finished
//     printf("current process is %d and finish time is %d\n", CurrProcess->id, FinishTime);
//     int prevTime = getClk();
//     CurrProcess->startTime = getClk(); //set start time for process
//     while(true){
//         if(getClk != prevTime){
//             printf("Current time is %d\n", getClk());
//             prevTime = getClk;
//         }
//         if(getClk() == FinishTime){
//             printf("process %d is finished\n", CurrProcess->id);
//             strcpy(CurrProcess->status,"finished");
//             break;
//         }
//     }
//     CurrProcess->finishTime = getClk(); //set finish time for process
// }

int main(int argc, char *argv[])
{
    initClk();
    quantum = atoi(argv[2]);
    processCount = atoi(argv[3]);
    algoChoice = atoi(argv[1]);
    printf("gowa scheduler algo choice is %d\n", algoChoice);
    printf("gowa scheduler quantum is %d\n", quantum);
    printf("gowa scheduler pCount is %d\n", processCount);
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
    if (algoChoice == 1)
    {
        //do FCFS
    }
    else if (algoChoice == 2)
    {
        // do SJF
    }
    else if (algoChoice == 3)
    {
        runHPF();
    }
    else if (algoChoice == 4)
    {
        // do SRTN
    }
    else if (algoChoice == 5)
    {
        // do RR
    }
    else
    {
        printf("Error no algorithm with this number\n");
        return 0;
    }
    printf("rec Done scheduler \n");
    destroyClk(true);
    exit(0);
    raise(SIGKILL);
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
}

void runHPF()
{
    struct process *processesArr = (struct process *)malloc(processCount * sizeof(struct process));
    struct Queue *readyQueue = Queue_Constructor();
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
                printf("processRec with id %d\n", processToReceive.p.id);
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
    }
    struct process *p;
    for (int i = 0; i < processCount; i++)
    {
        p = dequeue(readyQueue);
        printf("process Deq with id %d\n", p->id);
    }
}