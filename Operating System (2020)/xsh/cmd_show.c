#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 0x0FFF

// show stdin string.
int main()
{
    int retval = 0;
    char buf[BUFSIZE];

    memset(buf, 0, sizeof(buf));

    retval = read(STDIN_FILENO, buf, sizeof(buf));
    if (retval < 0)
        goto out;

    printf("GET: %s", buf);

    putchar('\n'); // put newline
    retval = 0;

out:
    return retval;
}
