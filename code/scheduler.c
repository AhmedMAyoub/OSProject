#include "headers.h"
#include <errno.h>

struct msgbuff
{
    long mtype;
    char mtext[256];
};

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
    printf("hena");
    switch (AlgChoice)
    {
    case 1:
        //FCFS
        printf("First come First serve");
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
    destroyClk(true);
    return 0;
}
