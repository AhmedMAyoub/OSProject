#include "headers.h"
#include <stdio.h>
#include <string.h>

int processCount = 0;
int clockPId;
int schedulerPId;
int currTime;

void clearResources(int);
void readInputFile(FILE *fp, char *fileName, struct process *processes);
void countProcesses(FILE *fp, char *fileName);
void split_string(char *line, struct process *process);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization

    // 1. Read the input files.
    FILE *fp;
    char *fileName = argv[1];
    countProcesses(fp, fileName);
    struct process processesArr[processCount];
    readInputFile(fp, fileName, processesArr);
    printf("Process Count %d\n", processCount);
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    char *schedAlgo = argv[3]; // Here  we take the number of the scheduling algorithm
    char *algoParams = argv[5];
    printf("Sched Algo number %s\n", schedAlgo);
    printf("Algo Parameters %s\n", algoParams);
    // 3. Initiate and create the scheduler and clock processes.
    clockPId = fork();
    if (clockPId == 0) //code to be executed if you are the clock child
    {
        char *args[] = {"./clk.out", NULL}; // using this to initialize clk.c
        execvp(args[0], args);
        exit(-1);
    }
    else if (clockPId == -1)
    {
        printf("Cannot create clock process");
        exit(0);
    }
    else
    { //code for process_generator after forking child
        initClk();
        schedulerPId = fork();
        if (schedulerPId == 0) // code if i am the scheduler child
        {
            char *pCount;
            sprintf(pCount, "%d", processCount);
            char *args[] = {"./scheduler.out", schedAlgo, algoParams, pCount, NULL}; // using this to initialize scheduelr.c
            printf("initializing scheduler\n");
            execvp(args[0], args);
            exit(-1);
        }
        else if (schedulerPId == -1)
        {
            printf("Cannot create scheduler process");
            exit(0);
        }
        else //code if i am the process_generator to keep checking the processes and send them to scheduler
        {
            key_t msgQScheduler;
            int rec_val, msgQSched_id;
            msgQScheduler = ftok("keyfile.txt", 33); // create unique key
            msgQSched_id = msgget(msgQScheduler, 0666 | IPC_CREAT);
            if (msgQSched_id == -1)
            {
                perror("Error in create");
            }
            printf("Message Queue ID = %d\n", msgQSched_id);
            printf("clock pid is %d\n", clockPId);
            printf("scheduler pid is %d\n", schedulerPId);
            // 4. Use this function after creating the clock process to initialize clock.
            int processNum = 0;
            int stat_locSched;
            int schPId = 0;
            struct msgBuffProcesses processToSend;
            processToSend.processtype = 1;
            processToSend.p = processesArr[0];
            int send_val;
            int processesSent = 0;
            while (1)
            {
                while (processesArr[processesSent].arrTime == getClk())
                {
                    processToSend.p = processesArr[processesSent];
                    send_val = msgsnd(msgQSched_id, &processToSend, sizeof(processToSend.p), IPC_NOWAIT);
                    processesSent++;
                }
                if (processesSent == processCount)
                {
                    processToSend.p.id = -1;
                    send_val = msgsnd(msgQSched_id, &processToSend, sizeof(processToSend.p), IPC_NOWAIT);
                    break;
                }
            }
            printf("sending done\n");
            schPId = waitpid(schedulerPId, &stat_locSched, 0);
            if (!(stat_locSched & 0x00FF))
            {
                //Scheduler finished and terminated with exit code
                printf("\nA Scheduler with pid %d terminated with exit code %d\n", schPId, stat_locSched >> 8);
                msgctl(msgQSched_id, IPC_RMID, (struct msqid_ds *)0);
                destroyClk(true);
            }
        }
    }
    // printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    return 0;
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    kill(SIGKILL, getpid());
}

void readInputFile(FILE *fp, char *fileName, struct process *processes)
{
    int num = 0;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    char string[100];
    fgets(string, 100, fp);
    while (fgets(string, 100, fp) != NULL)
    {
        split_string(string, &processes[num]);
        num++;
    }
    fclose(fp); //close file.
}

void split_string(char *line, struct process *process)
{

    const char delimiter[] = "\t";
    char *tmp;
    tmp = strtok(line, delimiter);
    if (tmp == NULL)
        return;
    process->id = atoi(tmp);
    for (int i = 0; i < 3; i++)
    {
        tmp = strtok(NULL, delimiter);
        if (tmp == NULL)
            break;
        if (i == 0)
        {
            process->arrTime = atoi(tmp);
        }
        if (i == 1)
        {
            process->runTime = atoi(tmp);
        }
        if (i == 2)
        {
            process->priority = atoi(tmp);
        }
    }
}

void countProcesses(FILE *fp, char *fileName)
{ //function counts number of processes
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    char chr = fgetc(fp);
    while (!feof(fp))
    {
        if (chr == '#')
        {
            while (chr != '\n')
            {
                chr = fgetc(fp);
            }
            chr = fgetc(fp);
        }
        //Count whenever new line is encountered
        if (chr == '\n' && chr != ' ')
        {
            processCount++;
        }
        //take next character from file.
        chr = fgetc(fp);
    }
    fclose(fp); //close file.
}