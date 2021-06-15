#include <string.h>
#include <math.h>>
#include <stdio.h>
#include "queue.h"
#include "priorityQueue.h"

int algoChoice = 0;
int quantum = 0;
int iwrite = 0;
int processCount = 0;
int currTime = 0;
int prevTime = 0;
bool isFinished = true;
int totalProcessesDone = 0;
int receivedP = 0;
int ihandler = 0;
int rec_val, msgQSched_id;

key_t msgQScheduler;
struct Queue *readyQueue;
struct pnode *priorityQHead = NULL;
struct pnode *currNode = NULL;
struct process *CurrProcess;
struct msgBuffProcesses processToReceive;
struct process *processesArr;
struct process *forkedProcesses;
FILE *fp;

void receiveInitialHPF();
void SJFProcess(struct process *CurrProcess);

//write to output file "results.txt"
void WriteFile(struct process *currProcess, int state)
{
    fp = fopen("./results.txt", "a");
    if (iwrite == 0)
    {
        fprintf(fp, "At  time  x  process  y  state  arr  w  total  z  remain  y  wait  k\n");
        iwrite++;
    }
    if (state == 1)
    {
        printf("byeegi hena?\n");
        fprintf(fp, "At  time  %d  process  %d  %s  arr  %d  total  %d  remain %d  wait  %d\n", getClk(), currProcess->id, currProcess->status, currProcess->arrTime, currProcess->totalTime, currProcess->remainingTime, currProcess->waitTime);
    }
    else if (state == 2)
    {
        printf("byeegi finish\n");
        int TA = currProcess->finishTime - currProcess->arrTime;
        int WTA = round(TA / currProcess->runTime);
        fprintf(fp, "At  time  %d  process  %d  %s  arr  %d  total  %d  remain %d  wait  %d  TA  %d  WTA  %d\n", getClk(), currProcess->id, currProcess->status, currProcess->arrTime, currProcess->totalTime, currProcess->remainingTime, currProcess->waitTime, TA, WTA);
    }
    fclose(fp);
}

//non-preemptive set start time
void setStartTime(struct process *currProcess)
{
    currProcess->startTime = getClk();
    currProcess->remainingTime = currProcess->runTime;
    currProcess->waitTime = currProcess->startTime - currProcess->arrTime;   //setting wait time of process in system
    currProcess->finishTime = currProcess->startTime + currProcess->runTime; //setting finish time of process
    currProcess->totalTime = currProcess->waitTime + currProcess->runTime;
    strcpy(currProcess->status, "running");
    WriteFile(currProcess, 1);
    //call write in file function
}
//non-preemptive set finish time
void setFinishTime(struct process *currProcess)
{
    currProcess->remainingTime = 0;
    strcpy(currProcess->status, "finished"); //setting status to finished
    WriteFile(currProcess, 2);
    //call write in file function
}
void FCFS(struct process *currProcess)
{
    CurrProcess = currProcess;
    isFinished = false;
    setStartTime(CurrProcess); //set start time for process
    int Processpid = fork();
    if (Processpid == 0)
    {
        Processpid = getpid();
        char remaining[20];
        sprintf(remaining, "%d", CurrProcess->remainingTime);
        char *args[] = {"./process.out", remaining, NULL};
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
    setFinishTime(CurrProcess);
    ihandler++;
}

void SJF();
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
        printf("ehna sjf\n");
        SJF();
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
    printf("GOWA HPF\n");
    // struct process* p;
    // for (int i=0; i<5; i++){
    //     printf("gowa loop \n");
    //     p = peek(&priorityQHead);
    //     pop(&priorityQHead);
    // }
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
    struct process *p;
    for (int i = 0; i < 5; i++)
    {
        printf("gowa loop \n");
        p = peek(&priorityQHead);
        printf("ana awel priority %d \n", p->priority);
        pop(&priorityQHead);
    }
}

void SJF()
{
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
            while (true)
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
                    if (priorityQHead == NULL)
                    {
                        priorityQHead = newNode(&processesArr[receivedP], processesArr[receivedP].runTime);
                        receivedP++;
                    }
                    else
                    {
                        push(&priorityQHead, &processesArr[receivedP], processesArr[receivedP].runTime);
                        receivedP++;
                    }
                }
            }
            struct process *sendP;
            if (!PQisEmpty(&priorityQHead) && isFinished)
            {
                sendP = peek(&priorityQHead);
                pop(&priorityQHead);
                SJFProcess(sendP);
            }
        }
    }
    struct process *sendP;
    while (totalProcessesDone != processCount)
    {
        if (!(priorityQHead == NULL) && isFinished)
        {
            sendP = peek(&priorityQHead);
            printf("processes done %d\n", totalProcessesDone);
            printf("id of p: %d\n", sendP->id);
            pop(&priorityQHead);
            SJFProcess(sendP);
        }
    }
}

void SJFProcess(struct process *currProcess)
{
    CurrProcess = currProcess;
    isFinished = false;
    setStartTime(CurrProcess); //set start time for process
    int Processpid = fork();
    if (Processpid == 0)
    {
        Processpid = getpid();
        char remaining[20];
        sprintf(remaining, "%d", CurrProcess->remainingTime);
        char *args[] = {"./process.out", remaining, NULL};
        printf("initializing process with id %d\n", CurrProcess->id);
        execvp(args[0], args);
    }
    else
    {
        return;
    }
    return;
}