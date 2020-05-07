#include <stdio.h>

// print arguments.
int main(int argc, char **argv)
{
    argc--;
    argv++;

    while (argc-- != 0)
        printf("%s ", *argv++);
    putchar('\n');

    return 0;
}
