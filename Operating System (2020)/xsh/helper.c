#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include "helper.h"

#define GETDIR(dir)                    \
    ((dir) == NULL) ? getenv("HOME") : \
    ((strcmp((dir), "~") == 0) ?       \
    getenv("HOME") : (dir))) == -1

static bool isprintable(char *str)
{
    char *ptr = str;
    while (*ptr != '\0')
    {
        if (!isprint(*ptr++))
            return false;
    }
    return true;
}

size_t __lenof__(void **p)
{
    size_t n = 0;
    while (*p++ != NULL)
        n++;
    return n;
}

void show_promot(int code)
{
    int uid = 0;
    char cwd[0xff] = {0}, *pcwd = cwd;
    char user[0xff] = {0};
    char home[0xff] = {0};
    char hostname[0xff] = {0};

    // uid
    uid = getuid();
    // cwd
    getcwd(cwd, sizeof(cwd) - 1);
    // user
    strncpy(user, getenv("USER"), sizeof(user) - 1);
    // home
    strncpy(home, getenv("HOME"), sizeof(home) - 1);
    // hostname
    gethostname(hostname, sizeof(hostname) - 1);

    if (strncmp(cwd, home, strlen(home)) == 0)
    {
        if (strcmp(cwd, home) == 0)
        {
            cwd[0] = '~';
            cwd[1] = '\0';
        }
        else if (cwd[strlen(home)] == '/')
        {
            pcwd = cwd + strlen(home) - 1;
            *pcwd = '~';
        }
    }

    if (!code)
        fprintf(stdout, "%s@%s (%s) %c> ",
                user, hostname, pcwd, (uid == 0) ? '$' : '#');
    else
        fprintf(stdout, "%s@%s (%s) [%d] %c> ",
                user, hostname, pcwd, code, (uid == 0) ? '$' : '#');
    fflush(stdout);
}

int read_line(char *buf, size_t size)
{
    int n = 0;
    int flag = 0;
    bool first = true;
    char *pbuf = buf;

    while (1)
    {
        *pbuf = getchar();

        if (*pbuf == EOF)
            _exit(EXIT_SUCCESS); /* exit on EOF */

        if (*pbuf == '\n' || pbuf - buf >= size)
            break;

        // comment support
        if (first && *pbuf == '#')
        {
            // eat rest buffer
            while (getchar() != '\n')
                ;
            return 0;
        }

        if (*pbuf == '\033')
            flag = 1;
        if (*pbuf == '[' && flag == 1)
            flag = 2;
        if (flag == 2)
        {
            switch (*pbuf)
            {
            case 'A':
            case 'B':
            case 'C':
            case 'D':
                /* ignore input */
                pbuf -= 3;
                /* fallthrough */
            default:
                /* set flag to zero */
                flag = 0;
                /* ignore others */
                break;
            }
        }

        if (*pbuf != ' ' && *pbuf != '\t' &&
            *pbuf != '\r' && *pbuf != '\0')
            first = false;
        pbuf++;
    }

    n = pbuf - buf;

    // if (fgets(buf, size, stdin) != NULL)
    // {
    //     n = strlen(buf);
    //     // removing trailing newline
    //     if (n > 0 && buf[n - 1] == '\n')
    //     {
    //         buf[--n] = '\0';
    //     }
    // }
    // else
    //     _exit(EXIT_SUCCESS); /* error occurs or EOF */

    return n;
}

int execute(char **argv, int mode, int *input, int *output)
{
    if (!isprintable(argv[0]))
        return EX_INVALID;

    int fd[2];

    /* Invoke pipe */
    pipe(fd);
    pid_t pid = fork();

    /*
        SCHEME:
            STDIN --> O --> O --> O --> STDOUT
    */

    if (pid == 0)
    {
        switch (mode)
        {
        case FIRST_CMD:
            dup2(fd[WRITE_END], STDOUT_FILENO);
            break;
        case MIDDLE_CMD:
            dup2(*input, STDIN_FILENO);
            dup2(fd[WRITE_END], STDOUT_FILENO);
            break;
        case LAST_CMD:
            dup2(*input, STDIN_FILENO);
            dup2(*output, STDOUT_FILENO);
            break;
        }

        if (execvp(argv[0], argv) != 0)
        {
            if (errno == ENOENT)
                fprintf(stderr, "Command not found: %s\n", argv[0]);
            else
                fprintf(stderr, "exec: %s\n", strerror(errno));
            _exit(EXIT_FAILURE);
        }
    }

    if (*input != STDIN_FILENO)
        close(*input);

    // Nothing more needs to be written
    close(fd[WRITE_END]);

    // If it's the last command, nothing more needs to be read
    if (mode == LAST_CMD)
        close(fd[READ_END]);

    *input = fd[READ_END];

    // 0
    return EX_SUCCESS;
}

int execute_builtin(char **argv)
{
    int retval = EX_SUCCESS;
    if (strcmp(argv[0], "exit") == 0)
    {
        if (argv[1] != NULL)
            retval = atoi(argv[1]);
        _exit(retval);
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        if (chdir(GETDIR(argv[1]))
        {
            fprintf(stderr, "cd: %s\n", strerror(errno));
            retval = errno;
        }
    }
    else if (*argv[0] == '#')
    {
        // ignore comments
        ;
    }
    else
    {
        retval = EX_BUILTIN;
    }

    return retval;
}

int parse_arguments(char *buf, char **argv)
{
    char *pbuf = buf;
    char **pargv = argv;

    while (*pbuf != '\0') /* if not the end of line */
    {
        while (*pbuf == ' ' || *pbuf == '\t' ||
               *pbuf == '\r' || *pbuf == '\n')
            *pbuf++ = '\0'; /* replace white spaces with 0 */
        if (*pbuf != '\0')
            *pargv++ = pbuf; /* save the argument position */
        while (*pbuf != '\0' && *pbuf != ' ' &&
               *pbuf != '\t' && *pbuf != '\r' && *pbuf != '\n')
            pbuf++; /* skip the argument until ... */
    }
    *pargv = NULL; /* mark the end of argument list */

    return lenof(argv);
}

int parse_commands(char *buf, char **argv, char ***cmds, char **redir, int *mode)
{
    char *pbuf = buf;
    char **pargv = argv;
    char ***pcmds = cmds;

    char *next = strchr(pbuf, '|');
    char *predir = strchr(pbuf, '>');

    if (predir != NULL)
    {
        *predir++ = '\0';

        // >> for append mode
        if (*predir == '>')
        {
            *mode = RE_APPEND_MODE;
            *predir++ = '\0';
        }
        else
        {
            *mode = RE_WRITE_MODE;
        }

        while (*predir != '\0')
        {
            // remove white spaces
            if (*predir == ' ' || *predir == '\t' ||
                *predir == '\r' || *predir == '\n')
                *predir = '\0';
            if (*redir == NULL && *predir != '\0')
                *redir = predir;
            predir++;
        }

        if (*redir == NULL || !strlen(*redir))
        {
            fprintf(stderr, "invalid fd redirect\n");
            return 0;
        }
    }

    while (1)
    {
        if (next != NULL)
            *next = '\0';

        // ignore empty
        if (!parse_arguments(pbuf, pargv))
        {
            if ((pcmds - cmds) > 0)
                fprintf(stderr, "invalid command\n");
            return 0;
        }

        *pcmds++ = pargv;
        pargv += lenof(pargv) + 1;

        if (next == NULL)
            break;

        pbuf = ++next;
        next = strchr(pbuf, '|');
    }

    return lenof(cmds);
}
