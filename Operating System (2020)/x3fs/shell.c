#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "fs.h"
#include "shell.h"

int argc = 0;
char *argv[BUFSIZE - 1] = {0};
char buf[BUFSIZE] = {0};

bool mounted = false;

int read_input(void)
{
    int retval = -1;
    char *pbuf = buf;

    memset(buf, 0, sizeof(buf));

    retval = read(STDIN_FILENO, buf, BUFSIZE - 1);

    if (retval == 0)
        exit(EXIT_SUCCESS);

    if (retval < 0)
        printf("parse argument failed: %s\n", strerror(-retval));
    return retval;
}

int parse_args(void)
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

    return (argc = pargv - argv);
}

void sh_promot()
{
    printf("(%s)> ", mounted ? get_dirname(cur_dir) : "NULL");
}

void sh_help()
{
    puts("Commands:");
    for (int i = 0; i < CMD_MAP_LEN - 1; ++i)
    {
        printf("%9s\t%s\n", cmd_map[i].cmd, cmd_map[i].desc);
    }
    return;
}

void sh_pwd()
{
}

void sh_mount(const char *filename)
{
    if (mounted)
    {
        puts("Current disk is in use");
        return;
    }
    if (access(filename, F_OK) < 0)
    {
        puts("Disk file not found");
        return;
    }

    if (fs_loadfrom(filename) > 0)
        mounted = true;
}

void sh_umount()
{
    if (!mounted)
    {
        puts("No disk mounted");
        return;
    }

    fs_writeto(NULL);
    mounted = false;
}

void sh_exit()
{
    if (mounted)
    {
        puts("Saving data to disk file...");
        fs_writeto(NULL);
    }

    exit(EXIT_SUCCESS);
}

int main()
{
    int retval = -1;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    int n = 0;
    char **p = argv;

    while (1)
    {
        sh_promot();

        if ((retval = read_input()) < 0)
            goto out;

        if (!parse_args())
            continue;

        int i = 0;
        char *cmd = argv[0];
        while (cmd && cmd_map[i].cmd)
        {
            if (!strcmp(cmd_map[i].cmd, cmd))
            {
                switch (cmd_map[i].type)
                {
                case TYPE_ARG0:
                    check_protexted_cmd(cmd_map[i]);
                    ((int (*)())(cmd_map[i].func))();
                    break;
                case TYPE_ARG1:
                    check_arg_length(2);
                    check_protexted_cmd(cmd_map[i]);
                    ((int (*)(char *, ...))(cmd_map[i].func))(argv[1]);
                    break;
                case 10:
                    check_arg_length(2);
                    check_protexted_cmd(cmd_map[i]);
                    ((int (*)(int, ...))(cmd_map[i].func))(atoi(argv[1]));
                    break;
                case TYPE_EXIT:
                    ((int (*)())(cmd_map[i].func))();
                    break;
                }
            out_loop:
                break;
            }
            ++i;
        }
        if (i + 1 >= CMD_MAP_LEN)
        {
            puts("shell: command not found");
        }
    }

out:
    return retval;
}
