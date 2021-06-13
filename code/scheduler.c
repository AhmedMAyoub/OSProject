#include "headers.h"
#include <errno.h>
#include <string.h>

struct msgbuff
{
    long mtype;
    char mtext[256];
};

struct process
{
    int id;
    int arrTime;
    int runTime;
    int waitTime;
    int totalTime;
    char status[50];
    int remainingTime;
};

void FCFS(struct process CurrProcess){
    int CurrClock = getClk(); //get current time when process enters
    printf("Curren time upon entering %d\n", CurrClock);
    strcpy(CurrProcess.status, "running"); //setting status of process to running
    int FinishTime = CurrClock + CurrProcess.runTime; //calculating when process should be finished
    printf("current process is %d and finish time is %d\n", CurrProcess.id, FinishTime);
    while(true){
        printf("Current time is %d\n", getClk());
        printf("Finish time is %d\n", FinishTime);
        if(getClk() == FinishTime){
            printf("process %d is finished\n", CurrProcess.id);
            strcpy(CurrProcess.status,"finished");
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    printf("scheduler is initialized\n");
    int AlgChoice = atoi(argv[1]);
    printf("AlgChoice is %d\n", AlgChoice);
    initClk();
    key_t key_id;
    int rec_val, msgq_id;
    key_id = ftok("keyfile.txt", 33);           // create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); //create message queue and return id

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("Message queue id successful\n");
    struct msgbuff process_det;
    while (1)
    {
        // receiving messages from msg queue
        printf("Receiving message\n");
        rec_val = msgrcv(msgq_id, &process_det, sizeof(process_det.mtext), 0, IPC_NOWAIT);
        if (rec_val == -1)
        {
            if (errno == ENOMSG)
            {
                break;
            }
            else
            {
                perror("Error in receive");
            }
        }
        else
        {
            printf("Process received");
        }
        
    }
    //TODO: implement the scheduler.
    struct process p1;
    struct process p2;
    struct process p3;
    p1.id = 1;
    p1.arrTime = 0;
    p1.runTime = 10;
    p2.id = 2;
    p2.arrTime = 1;
    p2.runTime = 3;
    p3.id = 3;
    p3.arrTime = 7;
    p3.runTime = 6;
    int n = 3;
    struct process processArray[3] = {p1, p2, p3};
    printf("hena\n");
    switch (AlgChoice)
    {
    case 1:
        //FCFS
        printf("First come First serve");
        for (int i=0; i<3; i++){
            FCFS(processArray[i]);
        }
        break;
    case 2:
        //SJF
        break;
    case 3:
        //HPF
        break;
    case 4:
        //SRTN
        break;
    case 5:
        //RR
        break;
    default:
        printf("invalid algorithm choice");
        break;
    }
    //TODO: upon termination release the clock resources.
    printf("khalasna");
    exit(-1);
    return 0;
}