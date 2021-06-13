#include "headers.h"
#include <stdio.h>

struct process {
    int id;
    int arrTime;
    int runTime;
    int waitTime;
    int totalTime;
    int remainingTime;
};

int processCount = 0;

void clearResources(int);
void readInputFile(FILE* fp, char*fileName, struct process* processes);
void countProcesses(FILE* fp, char*fileName);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization

    // 1. Read the input files.
    FILE *fp;
    char* fileName = "processes.txt";
    countProcesses(fp, fileName);
    struct process* processes[processCount];

    printf("%d\n", processCount);
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

void readInputFile(FILE* fp, char*fileName, struct process* processes) {
    fp = fopen(fileName, "r");
    char chr = fgetc(fp);
    while (!feof(fp))
    {
        if (chr == '#')
        {
            while (chr != '\n'){
                chr = fgetc(fp);
            }
            chr = fgetc(fp);
        }
        //Count whenever new line is encountered
        if (chr == '\n')
        {
            chr = fgetc(fp);
        }
        //take next character from file.
        chr = fgetc(fp);
    }
    fclose(fp); //close file.
}

void countProcesses(FILE* fp, char*fileName) {  //function counts number of processes
    fp = fopen(fileName, "r");
    char chr = fgetc(fp);
    while (!feof(fp))
    {
        if (chr == '#')
        {
            while (chr != '\n'){
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