#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "xdev.h"

int main(int argc, char *argv[])
{
    int fd, retval = -1;

    if (argc <= 1)
    {
        fprintf(stdout, "Usage: ./xdevctl [full|null|zero|inspect]\n");
        exit(0);
    }

    fd = open(DEVICE_FILE_NAME, 0);
    if (fd < 0)
    {
        fprintf(stderr, "cannot open device file: %s\n", DEVICE_FILE_NAME);
        exit(1);
    }

    argc--;
    argv++;

    if (!strcmp(argv[0], "full"))
    {
        if (ioctl(fd, IOCTL_FULL) < 0)
        {
            fprintf(stderr, "ioctl failed\n");
            goto out;
        }
        retval = 0;
    }
    else if (!strcmp(argv[0], "null"))
    {
        if (ioctl(fd, IOCTL_NULL) < 0)
        {
            fprintf(stderr, "ioctl failed\n");
            goto out;
        }
        retval = 0;
    }
    else if (!strcmp(argv[0], "zero"))
    {
        if (ioctl(fd, IOCTL_ZERO) < 0)
        {
            fprintf(stderr, "ioctl failed\n");
            goto out;
        }
        retval = 0;
    }
    else if (!strcmp(argv[0], "inspect"))
    {
        switch (ioctl(fd, IOCTL_INSPECT))
        {
        case IOCTL_FULL:
            printf("full mode\n");
            break;
        case IOCTL_NULL:
            printf("null mode\n");
            break;
        case IOCTL_ZERO:
            printf("zero mode\n");
            break;
        default:
            printf("unknown mode\n");
            break;
        }
        retval = 0;
    }
    else
    {
        fprintf(stderr, "unknown type: %s\n", argv[0]);
    }

out:
    close(fd);
    return retval;
}
