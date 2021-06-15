#include <string.h>
#include "queue.h"
#include "priorityQueue.h"

int algoChoice = 0;
int quantum = 0;
int processCount = 0;
int currTime = 0;
int prevTime = 0;
bool isFinished = true;
int totalProcessesDone = 0;
int receivedP = 0;
int rec_val, msgQSched_id;
struct process *currProcess = NULL;
void *shmaddr1;
int shmid;

key_t msgQScheduler;
struct Queue *readyQueue;
struct pnode *priorityQHead = NULL;
struct pnode *currNode = NULL;
struct msgBuffProcesses processToReceive;
struct process *processesArr;

void receiveInitialHPF();
void receiveHPF();
void setContinue();
void setProcessStart(int pid);
void setPause();

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
    readyQueue = Queue_Constructor();
    processesArr = (struct process *)malloc(processCount * sizeof(struct process));
    while (1)
    {
        currTime = getClk();
        if (currTime != prevTime)
        {
            if (receivedP == processCount) //will be changed once we implement algorithms
            {
                break;
                printf("total %d\n", totalProcessesDone);
            }
            processToReceive.processtype = 1;
            printf("time is %d\n", currTime);
            prevTime = currTime;
            while (1)
            {
                rec_val = msgrcv(msgQSched_id, &processToReceive, sizeof(processToReceive.p), 0, !IPC_NOWAIT);
                int printTime = getClk();
                if (processToReceive.p.id == -1)
                {
                    break;
                }
                else
                {
                    printf("processRec with id %d received at %d\n", processToReceive.p.id, printTime);
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
    printf("total %d\n", totalProcessesDone);
    print_Queue(readyQueue);
    while (totalProcessesDone != processCount)
    {
        if (isFinished && !(isEmpty(readyQueue)))
        {
            // printf("total %d\n", totalProcessesDone);
            FCFS(dequeue(readyQueue));
            //  print_Queue(readyQueue);
        }
    }
}

//First come first serve algorithm

void handleProcessFinish()
{
    printf("inside handler process\n");
    isFinished = true;
    totalProcessesDone = totalProcessesDone + 1;
    currProcess->finished = true;
    currProcess->finishTime = getClk();
    currProcess->totalTime = currProcess->finishTime - currProcess->arrTime;
    printf("Finished Process id is %d, arrTime is %d, finishTime is %d \n", currProcess->id, currProcess->arrTime, currProcess->finishTime);
    currProcess = NULL;
}

void HPF();

int main(int argc, char *argv[])
{
    initClk();
    signal(SIGUSR1, handleProcessFinish);
    algoChoice = atoi(argv[1]);
    quantum = atoi(argv[2]);
    processCount = atoi(argv[3]);
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
    printf("gowa scheduler algo choice is %d\n", algoChoice);
    printf("gowa scheduler quantum is %d\n", quantum);
    printf("gowa scheduler pCount is %d\n", processCount);
    switch (algoChoice)
    {
    case 1:
        ReceivingLoopFCFS();
        break;
    case 2:
        // do  SJF
        break;
    case 3:
        // do HPF
        printf("Ehna HPF\n");
        HPF();
        break;
    case 4:
        // do SRTN
        break;
    case 5:
        // do RR
        break;
    }
    printf("rec Done scheduler Algorithm 1 \n");
    destroyClk(true);
    exit(0);
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
}

void HPF()
{
    processesArr = (struct process *)malloc(processCount * sizeof(struct process));
    receiveInitialHPF();
    struct process *temp;
    int currProcessPId;
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
    while (totalProcessesDone != processCount) // not all processes finished yet
    {
        printf("Current Clock is %d\n", getClk());
        if (currProcess) {
            currProcess->remainingTime = currProcess->remainingTime - 1;
        }
        if (currProcess == NULL) // if no current running process take one from queue
        {
            currProcess = peek(&priorityQHead);
            pop(&priorityQHead);
            if (currProcess->isForked == true) // if chosen process was forked before ---> resume
            {
                setContinue();
            }
            else // if chosen process was not forked forked before ---> fork and set its initial state
            {
                currProcessPId = fork();
                if (currProcessPId == -1)
                {
                    perror("Error in fork!!");
                }
                else if (currProcessPId == 0) //inside process child
                {
                    printf("I am child with pid %d forking a new process with id %d\n", getpid(), currProcess->id);
                    char rTime[20];
                    sprintf(rTime, "%d", currProcess->runTime);
                    char id[20];
                    sprintf(id, "%d", currProcess->id);
                    char st[20];
                    sprintf(st, "%d", currProcess->startTime);
                    char *args[] = {"./process.out", st, rTime, id, NULL}; // initializing process
                    execvp(args[0], args);
                    exit(0);
                }
                else {
                    setProcessStart(currProcessPId);
                }
            }
        }
        else //there is a process already running
        {
            temp = peek(&priorityQHead);
            if (temp->priority < currProcess->priority)
            {
                printf("Found more prio\n");
                kill(currProcess->pid, SIGKILL);
                currProcess->isForked = false;
                printf("currRemainingTime is %d\n", currProcess->remainingTime);
                push(&priorityQHead, currProcess, currProcess->priority);
                setPause();
                currProcess = NULL; // check if process at head has higher priority
                // if higher then enqueue set curr process to null
            } // dequeue it from prioQ
            //////////////////////////////

            // if lower continue running until next clock cycle
        }
        if (currProcess)
        {
            printf("Current process id is %d\n", currProcess->id);
        }
        if (receivedP != processCount)
        {
            printf("Current process count is %d\n", receivedP);
            receiveHPF();
        }
    }
}

void receiveInitialHPF()
{
    while (true)
    {
        rec_val = msgrcv(msgQSched_id, &processToReceive, sizeof(processToReceive.p), 0, !IPC_NOWAIT);
        if (processToReceive.p.id == -1)
        {
            break;
        }
        else
        {
            processesArr[receivedP] = processToReceive.p;
            if (priorityQHead == NULL)
            {
                printf("ha3ml enqueue using new node \n");
                priorityQHead = newNode(&processesArr[receivedP], processesArr[receivedP].priority);
                receivedP++;
            }
            else
            {
                printf("ha3ml enqueue using push\n");
                push(&priorityQHead, &processesArr[receivedP], processesArr[receivedP].priority);
                receivedP++;
            }
        }
    }
}

void receiveHPF()
{
    while (true)
    {
        rec_val = msgrcv(msgQSched_id, &processToReceive, sizeof(processToReceive.p), 0, !IPC_NOWAIT);
        if (processToReceive.p.id == -1)
        {
            break;
        }
        else
        {
            processesArr[receivedP] = processToReceive.p;
            push(&priorityQHead, &processesArr[receivedP], processesArr[receivedP].priority);
            printf("enqueued %d\n", processesArr[receivedP].id);
            receivedP++;
        }
    }
}

void setProcessStart(int pid)
{
    currProcess->startTime = getClk();
    currProcess->waitTime = getClk() - currProcess->arrTime;
    char st[25] = "started";
    sprintf(currProcess->status, "%s", st);
    currProcess->remainingTime = currProcess->runTime;
    currProcess->isForked = true;
    currProcess->pid = pid;
    currProcess->finished = false;
    currProcess->lastStartTime = getClk();
}

void setContinue()
{
    printf("currProc resumed id %d\n", currProcess->id);
    printf("currProc resumed remTime %d\n", currProcess->remainingTime);
    currProcess->waitTime = currProcess->waitTime + getClk() - currProcess->lastStopTime;
    char st[25] = "resumed";
    sprintf(currProcess->status, "%s", st);
    printf("currProc status %s\n", currProcess->status);
    currProcess->lastStartTime = getClk();
}

void setPause() {
    printf("currProc Paused id %d\n", currProcess->id);
    char st[25] = "stopped";
    sprintf(currProcess->status, "%s", st);
    printf("currProc status %s\n", currProcess->status);
    currProcess->lastStopTime = getClk();
    currProcess->remainingTime = getClk() - currProcess->lastStartTime;
}