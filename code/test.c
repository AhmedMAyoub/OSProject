#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *args[] = {"./scheduler.out", NULL}; // using this to initialize scheduelr.c
    printf("initializing scheduler\n");
    execvp(args[0], args);
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
}