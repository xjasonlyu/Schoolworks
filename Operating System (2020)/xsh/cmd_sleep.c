#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT 10

int main(int argc, char **argv)
{
    argc--;
    argv++;

    int timeout = (argc == 0) ? DEFAULT : atoi(argv[0]);
    printf("Gonna sleep for %ds\n", timeout);

    // just sleep
    sleep(timeout);

    return 0;
}
