#include "headers.h"

void clearResources(int);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    // 3. Initiate and create the scheduler and clock processes.
    int clock_process = fork();
    if (clock_process == 0)
    {
        clock_process = getpid();
        char *args[] = {"./clk.out", NULL}; // using this to initialize clk.c
        printf("initializing clock\n");
        execvp(args[0], args);
    }
    else
    {
        int scheduler_process = fork();
        if (scheduler_process == 0)
        {
            scheduler_process = getpid();
            char *args[] = {"./scheduler.out", "1", NULL}; // using this to execute scheduler.c
            printf("initializing scheduler\n");
            execvp(args[0], args);
            exit(-1);
        }
        else
        {
            // 4. Use this function after creating the clock process to initialize clock.
            initClk();
            // To get time use this function.
            int x = getClk();
            printf("Current Time is %d\n", x);
            // TODO Generation Main Loop
            // 5. Create a data structure for processes and provide it with its parameters.
            // 6. Send the information to the scheduler at the appropriate time.
            // 7. Clear clock resources
             int exit_val;
            waitpid(scheduler_process, &exit_val, 0);
            while (true)
            {
                if (exit_val >> 8 == -1)
                {
                    printf("%d\n", exit_val>>8);
                    destroyClk(true);
                    break;
                }
            }
            return 0;
        }
    }
    return 0;
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
