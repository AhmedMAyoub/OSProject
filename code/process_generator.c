#include "headers.h"
#include <stdio.h>
#include <string.h>

struct process
{
    int id;
    int arrTime;
    int runTime;
    int waitTime;
    int totalTime;
    int remainingTime;
    int priority;
    char status[50];
};

int processCount = 0;

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
    char *fileName = "processes.txt";
    countProcesses(fp, fileName);
    struct process processesArr[processCount];
    readInputFile(fp, fileName, processesArr);
    printf("%d\n", processCount);
    for (int i=0; i<processCount; i++) {
        printf("ID of process is %d\n", processesArr[i].id);
        printf("ArrTime of process is %d\n", processesArr[i].arrTime);
        printf("RunTime of process is %d\n", processesArr[i].runTime);
        printf("Priority of process is %d\n\n", processesArr[i].priority);
    }
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock.
    // initClk();
    // To get time use this function.
    // int x = getClk();
    // printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    // destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    kill(SIGKILL, getpid());
}

void readInputFile(FILE *fp, char *fileName, struct process *processes)
{
    int num =0;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    char string[100];
    fgets(string, 100, fp);
    while (fgets(string, 100, fp) != NULL) {
        split_string(string, &processes[num]);
        num++;
    }
    fclose(fp); //close file.
}

void split_string(char *line, struct process *process) {

    const char delimiter[] = "\t";
    char *tmp;
    tmp = strtok(line, delimiter);
    if (tmp == NULL)
    return;
    process->id = atoi(tmp);
    for (int i=0;i<3;i++) {
        tmp = strtok(NULL, delimiter);
        if (tmp == NULL)
            break;
        if (i ==0){
        process->arrTime = atoi(tmp);
        }
        if (i ==1){
        process->runTime = atoi(tmp);
        }
        if (i ==2){
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