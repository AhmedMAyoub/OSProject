#include "headers.h"
#include <stdio.h>
#include <string.h>

int processCount = 0;
int clockPId;
int schedulerPId;

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
    int schedAlgo = atoi(argv[3]); // Here  we take the number of the scheduling algorithm
    int algoParams = atoi(argv[5]);
    printf("Sched Algo number %d\n", schedAlgo);
    printf("Algo Parameters %d\n", algoParams);
    // 3. Initiate and create the scheduler and clock processes.
    clockPId = fork();
    if (clockPId == 0) //code to be executed if you are the clock child
    {
        char *args[] = {"./clk.out", NULL}; // using this to initialize clk.c
        printf("initializing clock\n");
        execvp(args[0], args);
    }
    else
    { //code for process_generator
        initClk();
        key_t msgQScheduler;
        int rec_val, msgQSched_id;
        msgQScheduler = ftok("keyfile.txt", 33); // create unique key
        msgQSched_id = msgget(msgQScheduler, 0666 | IPC_CREAT);
        schedulerPId = fork();
        if (schedulerPId == 0) // code if i am the scheduler child
        {
            char *args[] = {"./scheduler.out", schedAlgo, algoParams, NULL}; // using this to initialize scheduelr.c
            printf("initializing clock\n");
            execvp(args[0], args);
        }
        else //code if i am the process_generator to keep checking the processes and send them to scheduler
        {
            // 4. Use this function after creating the clock process to initialize clock.
            int processNum=0;
            int stat_locSched;
            int schPId =0;
            while (1)
            {
                if(processesArr[processNum].arrTime == getClk()){
                    // Send process to scheduler
                    processNum++;
                }
                schPId = waitpid(schedulerPId, &stat_locSched, WNOHANG);
                if(schPId != 0 && schPId != -1) {
                    //Scheduler finished and terminated with exit code
                    printf("\nA Scheduler with pid %d terminated with exit code %d\n", schPId, stat_locSched>>8);
                    msgctl(msgQSched_id, IPC_RMID, (struct msqid_ds *)0);
                    break;
                }

            }
            destroyClk(true);
        }
    }
    // printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
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