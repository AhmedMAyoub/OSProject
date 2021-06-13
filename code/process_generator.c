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
int readInputFile();
void countProcesses(FILE* fp, char*fileName);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization

    // 1. Read the input files.
    FILE *fp;
    char* fileName = "processes.txt";
    countProcesses(fp, fileName);
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
}

int readInputFile() {
    
}

void countProcesses(FILE* fp, char*fileName) {  //function counts number of lines in file and subtracts one for header file to count proccesses
    fp = fopen(fileName, "r");
    if(fp == NULL){
        printf("Could not open file %s",fileName);
    }
    else {
        char ch;
        while(!feof(fp)){
            ch = fgetc(fp);
            if (ch == '\n'){
                processCount++;
            }
        }
        processCount--;
    }
    fclose(fp);
}