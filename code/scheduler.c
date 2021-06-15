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

key_t msgQScheduler;
struct Queue *readyQueue;
struct pnode *priorityQHead = NULL;
struct pnode *currNode = NULL;
struct msgBuffProcesses processToReceive;
struct process *processInRun;
struct process *processesArr; 
FILE* outputFile;

void receiveInitialHPF();

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
}

void HPF();


////////////////
///RoundRobin///
////////////////
void sigRR(int);
int stat_loc;
struct Queue*readyQueue ;
struct Queue*finishedQueue;
int flag=0;
int check=-1;
void RoundRobin(int numOfProcesses,int quant){
    //initzalling of the outputfile
    outputFile=fopen("scheduler.log","w");
    fprintf(outputFile,"#At time x process y state arr w total z remain y wait k\n");
    processCount=numOfProcesses;
    signal(SIGCHLD,sigRR);
    readyQueue=Queue_Constructor();
    finishedQueue=Queue_Constructor();
    processInRun->state=-1;
    while(processCount>0)
    {
        int recval=msgrcv(msgQSched_id,&processToReceive,sizeof(processToReceive.p),0, IPC_NOWAIT | MSG_NOERROR);
        if(recval != -1)
        {
            if(!(processToReceive.p.id = check)){
                check=processToReceive.p.id;
                processToReceive.p.remainingTime=processToReceive.p.runTime;
                processToReceive.p.state=0;
                processToReceive.p.waitTime=0;
                fprintf(outputFile,"At time %d process %d arrived arr %d total %d remain %d wait %d\n",getClk(),processToReceive.p.id,processToReceive.p.arrTime,processToReceive.p.runTime,processToReceive.p.runTime,0);
                enqueue(readyQueue,&processToReceive.p);
            }
        }
        if(processInRun->state==-1)
        {
            if(readyQueue->headPtr != NULL)
            {
                if(readyQueue->headPtr->processObj->state==2)
                {
                    processInRun=readyQueue->headPtr->processObj;
                    processInRun->state=1;
                    dequeue(readyQueue);
                    processInRun->resume-getClk();
                    processInRun->waitTime += (getClk()- processInRun->stopTime);
                    processInRun->totalwaitTime+=processInRun->waitTime;
                    fprintf(outputFile,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk,processInRun->id,processInRun->arrTime,processInRun->runTime,processInRun->remainingTime,processInRun->totalwaitTime);
                    shMemory(processInRun->waitTime);
                    kill(processInRun->pid,SIGCONT);
                }else if(readyQueue->headPtr->processObj->state==0)
                {
                    processInRun=readyQueue->headPtr->processObj;
                    processInRun->state=1;
                    processInRun->startTime=getClk();
                    processInRun->totalwaitTime=processInRun->startTime - processInRun->arrTime;
                    fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),processInRun->id,processInRun->arrTime,processInRun->runTime,processInRun->remainingTime,processInRun->totalwaitTime);
                    shMemory(0);
                    int pid=fork();
                    if (pid==0)
                    {
                        
                    }
                    processInRun->pid=pid;
                    dequeue(readyQueue);
                }
            }
        }
        else if(processInRun->state ==1 )
        {
            if(getClk()-processInRun->resume >= quant)
            {
                if(readyQueue->headPtr!=NULL)
                {
                    kill(processInRun->pid,SIGSTOP);
                    processInRun->state=2;
                    processInRun->stopTime=getClk();
                    processInRun->remainingTime=processInRun->remainingTime - quant;
                    fprintf(outputFile,"At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk(),processInRun->id,processInRun->arrTime,processInRun->runTime,processInRun->remainingTime,processInRun->totalwaitTime);
                    enqueue(readyQueue,processInRun);
                    if(readyQueue->headPtr->processObj->state==2)
                    {
                        processInRun=readyQueue->headPtr->processObj;
                        processInRun->state=1;
                        dequeue(readyQueue);
                        processInRun->resume=getClk();
                        processInRun->waitTime+=(getClk()-processInRun->stopTime);
                        processInRun->totalwaitTime += processInRun->waitTime;
                        kill(processInRun->pid,SIGCONT);
                    }
                    else if(readyQueue->headPtr->processObj->state == 0)
                    {
                        processInRun=readyQueue->headPtr->processObj;
                        processInRun->state=1;
                        processInRun->startTime=getClk();
                        processInRun->totalwaitTime=processInRun->startTime-processInRun->arrTime;
                        fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),processInRun->id,processInRun->arrTime,processInRun->runTime,processInRun->remainingTime,processInRun->totalwaitTime);
                        shMemory(0);
                        dequeue(readyQueue);
                    }
                }
                else
                {
                    processInRun->resume=getClk();
                    processInRun->remainingTime=processInRun->remainingTime-quant;
                }
            }
        }

    }

}
void sigRR(int signal)
{
    if(waitpid(-1,&stat_loc,WNOHANG))
    {
        if (WIFEXITED(stat_loc))
        {
            processInRun->finishTime = WEXITSTATUS(stat_loc);
            processInRun->turnaround = processInRun->finishTime - processInRun->arrTime;
            processInRun->totalwaitTime = processInRun->finishTime-processInRun->runTime - processInRun->arrTime;
            processInRun->wta = (float)processInRun->turnaround / (float)processInRun->runTime;
            processInRun->remainingTime = 0;
            processInRun->state = -1; 
            fprintf(outputFile,"At time %d process %d finished arr %d  remain %d wait %d ",processInRun->finishTime,processInRun->id,processInRun->arrTime,processInRun->remainingTime,processInRun->waitTime);
            fflush(stdout);   
            fprintf(outputFile,"TA %d WTA %0.2f \n",processInRun->turnaround,processInRun->wta);
            fflush(stdout);          
            enqueue(finishedQueue,processInRun);
            processCount--;                 
            flag =0; 

        }

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
        RoundRobin(processCount,quantum);
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
