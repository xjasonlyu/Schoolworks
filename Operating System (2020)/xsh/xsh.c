#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "helper.h"

// number of children
static int pcount = 0;

// last subprocess exit code
static int last_retval = 0;

// redirect file descreption
char *fredir = {0};
static int fmode = RE_DEFAULT_MODE;

// global variables
char buf[0x1000] = {0};
char *arguments[0x0fff] = {0};
char **commands[0x0fff] = {0};

#define CLEANUP()                                \
    do                                           \
    {                                            \
        fredir = NULL;                           \
        fmode = RE_DEFAULT_MODE;                 \
        memset(buf, 0, sizeof(buf));             \
        memset(arguments, 0, sizeof(arguments)); \
        memset(commands, 0, sizeof(commands));   \
    } while (0)

#define WAITN(n)                                                 \
    do                                                           \
    {                                                            \
        for (int i = 0; i < (n); i++)                            \
        {                                                        \
            if (wait(&last_retval) == -1 && errno)               \
                fprintf(stderr, "wait: %s\n", strerror(-errno)); \
            last_retval = WEXITSTATUS(last_retval);              \
        }                                                        \
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
        if (parse_commands(buf, arguments, commands, &fredir, &fmode) <= 0)
            continue;

        if (process() < 0)
            goto out;

        CLEANUP();
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
    show_promot(last_retval);
}

int process(void)
{
    int retval = -EFAULT;

    pcount = 0;
    const size_t cmdlen = lenof(commands);

    if (!cmdlen)
        goto out;

    int input = STDIN_FILENO;
    int output = STDOUT_FILENO;
    int mode = FIRST_CMD;

    // redirect fd exists
    if (fredir != NULL && strlen(fredir) > 0)
    {
        output = open(fredir, fmode, 0644);

        if (output < 0)
        {
            fprintf(stderr, "Cannot open file: %s\n", fredir);
            goto out;
        }
    }

    for (int i = 0; i < cmdlen; i++)
    {
        if (i == cmdlen - 1)
            mode = LAST_CMD;
        else
            mode = MIDDLE_CMD;

        // try if built-in first
        retval = excute_builtin(commands[i]);
        if (retval == EX_BUILTIN)
        {
            // excute as external command
            retval = excute(commands[i], mode, &input, &output);
            if (retval == EX_SUCCESS)
                pcount++;
            else
                break;
        }
    }

    // wait all children processes to complete
    WAITN(pcount);

    // close opened file
    if (output != STDOUT_FILENO)
        close(output);

out:
    return retval;
}

int parse(void)
{
    int retval = -EFAULT;

    // clear global variables
    CLEANUP();

    // set to zero
    pcount = 0;

    retval = read_line(buf, sizeof(buf) - 1);
    if (retval <= 0)
        goto out;

    retval = parse_commands(buf, arguments, commands, &fredir, &fmode);

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

    while (1) /* infinite loop */
    {
        show_promot(last_retval); /* print shell promot */

        if ((parse()) <= 0)
            continue; /* ignore */

        if (process() < 0)
            continue;
    }

    return 0;
}
