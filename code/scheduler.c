#include "queue.h"

int algoChoice = 0;
int quantum = 0;
int processCount = 0;

int main(int argc, char *argv[])
{
    initClk();
    struct Queue *processQueue = Queue_Constructor();
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
    processToReceive.processtype = 1;
    struct process *p;
    while (1)
    {
        printf("time is %d\n", getClk());
        // while (receivedP < processCount)
        // {
        //     rec_val = msgrcv(msgQSched_id, &processToReceive, sizeof(processToReceive.p), 0, !IPC_NOWAIT);
        //     if (rec_val == -1)
        //     {
        //         perror("Error in receive");
        //     }
        //     else
        //     {
        //         printf("new process rec with processId %d", processToReceive.p.id);
        //         receivedP++;
        //     }
        // }

        rec_val = msgrcv(msgQSched_id, &processToReceive, sizeof(processToReceive.p), 0, !IPC_NOWAIT);
        printf("processRec with id %d\n", processToReceive.p.id);
        printf("processRec with arrTime %d\n", processToReceive.p.arrTime);
        if (processToReceive.p.id == -1)
        {
            break;
        }
        else
        {
            enqueue(processQueue, &processToReceive.p);
            struct process *p = peekFront(processQueue);
            printf("Front pid is %d\n", p->id);
        }
    }
    for (int i = 0; i < processCount-1; i++)
    {
        p = dequeue(processQueue);
        printf("Dequeqing pid is %d\n", p->id);
    }
    if (isEmpty(processQueue))
    {
        printf("queue is now empty\n");
    }
    printf("rec Done scheduler \n");
    // destroyClk(true);
    exit(0);
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
}