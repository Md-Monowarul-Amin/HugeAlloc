#define _GNU_SOURCE
#include <err.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    int           parentCPU, childCPU;
    cpu_set_t     set;
    unsigned int  nloops;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s parent-cpu child-cpu num-loops\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    parentCPU = atoi(argv[1]);
    childCPU = atoi(argv[2]);
    nloops = atoi(argv[3]);

    CPU_ZERO(&set);

    switch (fork()) {
    case -1:            /* Error */
        err(EXIT_FAILURE, "fork");

    case 0:             /* Child */
        CPU_SET(childCPU, &set);

        if (sched_setaffinity(getpid(), sizeof(set), &set) == -1)
            err(EXIT_FAILURE, "sched_setaffinity");

        for (unsigned int j = 0; j < nloops; j++)
            getppid();

        exit(EXIT_SUCCESS);

    default:            /* Parent */
        CPU_SET(parentCPU, &set);

        if (sched_setaffinity(getpid(), sizeof(set), &set) == -1)
            err(EXIT_FAILURE, "sched_setaffinity");

        for (unsigned int j = 0; j < nloops; j++)
            getppid();

        wait(NULL);     /* Wait for child to terminate */
        exit(EXIT_SUCCESS);
    }
}