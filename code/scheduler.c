#include <string.h>
#include <math.h>
#include <stdio.h>
#include "queue.h"
#include "priorityQueue.h"
#include <math.h>

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
struct process *currProcess = NULL;
void *shmaddr1;
int shmid;
FILE *fp;
int avgWTA;
int avgWait;

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
void receiveHPF();
void setContinue();
void setProcessStart(int pid);
void setPause();
void HPF();
void SRTN();
void receiveInitialSRTN();
void receiveSRTN();
void writePerf();

//write to output file "results.txt"
void WriteFile(struct process *currProcess, int state)
{
    fp = fopen("./scheduler.log", "a");
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

void WriteFilePreEmp(struct process *currProcess, int state)
{
    fp = fopen("./scheduler.log", "a");
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
    fp = fopen("./scheduler.log", "w");
    fclose(fp);
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

void SJF();

void handleProcessFinish()
{
    if (algoChoice == 1 || algoChoice == 2)
    {
        printf("inside handler process\n");
        isFinished = true;
        totalProcessesDone = totalProcessesDone + 1;
        setFinishTime(CurrProcess);
        ihandler++;
    }
    else
    {
        printf("inside handler process\n");
        isFinished = true;
        totalProcessesDone = totalProcessesDone + 1;
        currProcess->finished = true;
        currProcess->finishTime = getClk();
        currProcess->totalTime = currProcess->finishTime - currProcess->arrTime;
        char st[25] = "finished";
        sprintf(currProcess->status, "%s", st);
        printf("Finished Process id is %d, arrTime is %d, finishTime is %d \n", currProcess->id, currProcess->arrTime, currProcess->finishTime);
        currProcess->remainingTime = 0;
        WriteFile(currProcess, 2);
        avgWait = avgWait + currProcess->waitTime;
        currProcess = NULL;
    }
}

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
        printf("Ehna SRTN\n");
        SRTN();
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
    fp = fopen("./scheduler.log", "w");
    fclose(fp);
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
        if (getClk() != prevTime)
        {
            prevTime = getClk();
            if (currProcess)
            {
                currProcess->remainingTime = currProcess->remainingTime - 1;
            }
            if (currProcess == NULL) // if no current running process take one from queue
            {
                currProcess = peek(&priorityQHead);
                pop(&priorityQHead);
                if (currProcess->isForked == true) // if chosen process was forked before ---> resume
                {
                    kill(currProcess->pid, SIGCONT);
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
                        char *args[] = {"./process.out", rTime, NULL}; // initializing process
                        execvp(args[0], args);
                        exit(0);
                    }
                    else
                    {
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
                    kill(currProcess->pid, SIGSTOP);
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
    writePerf();
}

void SRTN()
{
    fp = fopen("./scheduler.log", "w");
    fclose(fp);
    processesArr = (struct process *)malloc(processCount * sizeof(struct process));
    receiveInitialSRTN();
    struct process *temp;
    int currProcessPId;
    while (totalProcessesDone <= processCount) // not all processes finished yet
    {
        if (getClk() != prevTime)
        {
            prevTime = getClk();
            if (currProcess)
            {
                if (currProcess->remainingTime != 0)
                {
                    currProcess->remainingTime = currProcess->remainingTime - 1;
                }
            }
            if (currProcess == NULL) // if no current running process take one from queue
            {
                currProcess = peek(&priorityQHead);
                pop(&priorityQHead);
                if (currProcess->isForked == true) // if chosen process was forked before ---> resume
                {
                    kill(currProcess->pid, SIGCONT);
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
                        char *args[] = {"./process.out", rTime, NULL}; // initializing process
                        execvp(args[0], args);
                        exit(0);
                    }
                    else
                    {
                        setProcessStart(currProcessPId);
                    }
                }
            }
            else //there is a process already running
            {
                temp = peek(&priorityQHead);
                if (temp->remainingTime < currProcess->remainingTime)
                {
                    printf("Found more prio\n");
                    kill(currProcess->pid, SIGSTOP);
                    printf("currRemainingTime is %d\n", currProcess->remainingTime);
                    push(&priorityQHead, currProcess, currProcess->remainingTime);
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
                receiveSRTN();
            }
        }
    }
    writePerf();
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
            processesArr[receivedP].totalTime = 0;
            if (priorityQHead == NULL)
            {
                priorityQHead = newNode(&processesArr[receivedP], processesArr[receivedP].priority);
                receivedP++;
            }
            else
            {
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
            processesArr[receivedP].totalTime = 0;
            push(&priorityQHead, &processesArr[receivedP], processesArr[receivedP].priority);
            printf("enqueued %d\n", processesArr[receivedP].id);
            receivedP++;
        }
    }
}

void receiveInitialSRTN()
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
            processesArr[receivedP].remainingTime = processesArr[receivedP].runTime;
            processesArr[receivedP].totalTime = 0;
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
}

void receiveSRTN()
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
            processesArr[receivedP].remainingTime = processesArr[receivedP].runTime;
            processesArr[receivedP].totalTime = 0;
            push(&priorityQHead, &processesArr[receivedP], processesArr[receivedP].runTime);
            printf("enqueued %d\n", processesArr[receivedP].id);
            receivedP++;
        }
    }
}

void SJF()
{
    fp = fopen("./scheduler.log", "w");
    fclose(fp);
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
    currProcess->totalTime=0;
    WriteFile(currProcess, 1);
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
    WriteFile(currProcess, 1);
}

void setPause()
{
    printf("currProc Paused id %d\n", currProcess->id);
    char st[25] = "stopped";
    sprintf(currProcess->status, "%s", st);
    printf("currProc status %s\n", currProcess->status);
    currProcess->lastStopTime = getClk();
    WriteFile(currProcess, 1);
}

void writePerf()
{
    printf("AVG WTA %d and avg wait %d\n", avgWTA, avgWait);
}
