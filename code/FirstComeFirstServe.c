#include "headers.h";


struct process
{
    int id;
    int arrTime;
    int runTime;
    int waitTime;
    int totalTime;
    int remainingTime;
};

void main() {
    //initialize clock 
    initClk();

    // creating processes for testing
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
    //functions for scheduling
    while(n!=0){
        
    }
}