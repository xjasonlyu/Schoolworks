#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "helper.h"

// number of children
static int pcount = 0;

// global variables
char buf[0x1000] = {0};
char *arguments[0x0fff] = {0};
char **commands[0x0fff] = {0};

#define clear_buffer()                           \
    do                                           \
    {                                            \
        memset(buf, 0, sizeof(buf));             \
        memset(arguments, 0, sizeof(arguments)); \
        memset(commands, 0, sizeof(commands));   \
    } while (0)

// parse command in buf
int parse(void);

// process command in buf
int process(void);

// init shell via .xshrc
int init_shell(void);

// signal fallback handler
void sig_handler(int);

int init_shell(void)
{
    FILE *fp = fopen(".xshrc", "r");

    if (fp == NULL)
        goto out;

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        parse_commands(buf, arguments, commands);
        if (process() < 0)
            goto out;

        clear_buffer();
    }

    fclose(fp);

out:
    return 0; /* do nothing */
}

void sig_handler(int signo)
{
    if (signo != SIGINT)
        return;

    if (pcount > 0)
        return;

    putchar('\n');
    show_promot();
}

int process(void)
{
    int retval = -EFAULT;

    pcount = 0;
    const size_t cmdlen = lenof(commands);

    int input = 0;
    int mode = FIRST_CMD;

    if (!cmdlen)
        goto out;

    for (int i = 0; i < cmdlen; i++)
    {
        if (i == cmdlen - 1)
            mode = LAST_CMD;
        else
            mode = MIDDLE_CMD;

        // try if built-in first
        retval = excute_builtin(commands[i]);
        if (retval == NON_BUILTIN)
        {
            // excute as external command
            retval = excute(commands[i], &input, mode);
            if (retval == 0)
                pcount++;
        }
    }

    // wait all children processes to complete
    waitn(pcount);

out:
    return retval;
}

int parse(void)
{
    int retval = -EFAULT;

    // clear global variables
    clear_buffer();

    // set to zero
    pcount = 0;

    retval = read_line(buf, sizeof(buf) - 1);
    if (retval <= 0)
        goto out;

    retval = parse_commands(buf, arguments, commands);

out:
    if (retval < 0)
        printf("parse argument failed: %s\n", strerror(-retval));
    return retval;
}

int main()
{
    // handle SIGINT signal
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        die(EXIT_FAILURE, "Can't catch signal");

    // init
    if (init_shell() < 0)
        die(EXIT_FAILURE, "Init shell failed");

    while (1)
    {
        show_promot(); /* print shell promot */

        if ((parse()) <= 0)
            continue; /* ignore */

        if (process() < 0)
            continue;
    }

    return 0;
}
