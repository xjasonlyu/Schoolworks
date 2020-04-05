#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>

#ifndef SYS_renice
#define SYS_renice 333
#endif

int main(int argc, char **argv)
{
    int pid;
    int flag = 0;
    int nice = 0;

    int error;
    int u_nice;
    int u_prio;

    argc--;
    argv++;

    if (argc != 1 && argc != 2)
    {
        printf("Usage: renice2 pid [nice]\n");
        return 0;
    }

    pid = atoi(*argv);

    if (argc == 1)
    {
        flag = 0; // get nice
    }
    else
    {
        flag = 1; // set nice
        nice = atoi(*(++argv));
    }

    if (nice < -20 || nice > 19)
        // nice validation check
        error = -EFAULT;
    else
        // make syscall
        error = syscall(SYS_renice, pid, flag, nice, &u_prio, &u_nice);

    if (error)
        printf("renice2: failed to get nice for %d, error: %d\n", pid, error);
    else if (flag == 0)
        printf("%d (process ID) current nice is %d, prio is %d\n", pid, u_nice, u_prio);
    else if (flag == 1)
        printf("%d (process ID) set nice to %d, prio to %d\n", pid, u_nice, u_prio);

    return error;
}
