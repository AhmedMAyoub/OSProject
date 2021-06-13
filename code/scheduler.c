#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();
    printf("Chosen Algo is %d", atoi(argv[1]));
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    destroyClk(true);
}
