#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

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
        // exit(EXIT_SUCCESS);
        exit_shell();

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

void sh_init()
{
    return;
}

void sh_promot()
{
    printf("(%s)> ", mounted ? get_abspath(cur_dir) : "NULL");
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
    puts(get_abspath(cur_dir));
}

void sh_mount(const char *filename)
{
    if (mounted)
    {
        fprintf(stderr, "Current disk is in use\n");
        return;
    }
    if (access(filename, F_OK) < 0)
    {
        fprintf(stderr, "Disk file not found\n");
        return;
    }

    if (fs_loadfrom(filename) > 0)
        mounted = true;
}

void sh_umount()
{
    if (!mounted)
    {
        fprintf(stderr, "No disk mounted\n");
        return;
    }

    fs_writeto(NULL);
    mounted = false;
}

void sh_cat(const char *filename)
{
    int _fd = fs_open(filename);
    if (_fd < 0)
    {
        fprintf(stderr, "cat: cannot open %s\n", filename);
        return;
    }

    int n = 0;
    int size = ofs[_fd].fcb.size;
    char *b = malloc(BLOCK_SIZE);

    while (size > 0)
    {
        if ((n = fs_read(_fd, b, BLOCK_SIZE)) < 0)
        {
            fprintf(stderr, "cat: read error\n");
            break;
        }
        write(STDOUT_FILENO, b, n);
        size -= BLOCK_SIZE;
    }

    free(b);
    fs_close(_fd);
}

void sh_cpi(const char *src, const char *dst)
{
    int src_fd = open(src, O_RDONLY);
    if (src_fd < 0)
    {
        fprintf(stderr, "cpi: cannot open %s\n", src);
        return;
    }

    if (fs_create(dst) < 0)
    {
        fprintf(stderr, "cpi: cannot create %s\n", dst);
        close(src_fd);
        return;
    }

    int dst_fd = fs_open(dst);
    if (dst_fd < 0)
    {
        fprintf(stderr, "cpi: cannot open %s\n", dst);
        close(src_fd);
        return;
    }

    void *b = malloc(BLOCK_SIZE);
    int n = 0;
    do
    {
        n = read(src_fd, b, BLOCK_SIZE);
        fs_write(dst_fd, b, n);
    } while (n > 0);

    free(b);
    close(src_fd);
    fs_close(dst_fd);
}

void sh_cpo(const char *src, const char *dst)
{
    if (access(dst, F_OK) != -1)
    {
        fprintf(stderr, "cpo: %s: file exists\n", dst);
        return;
    }

    int src_fd = fs_open(src);
    if (src_fd < 0)
    {
        fprintf(stderr, "cpo: cannot open %s\n", src);
        return;
    }

    int dst_fd = open(dst, O_WRONLY | O_CREAT,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (dst_fd < 0)
    {
        fprintf(stderr, "cpo: cannot create %s\n", dst);
        fs_close(src_fd);
        return;
    }

    void *b = malloc(BLOCK_SIZE);
    int n = 0;
    do
    {
        n = fs_read(src_fd, b, BLOCK_SIZE);
        write(dst_fd, b, n);
    } while (n > 0);

    free(b);
    close(dst_fd);
    fs_close(src_fd);
}

void sh_exit()
{
    exit_shell();
}

int main()
{
    int retval = -1;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    int n = 0;
    char **p = argv;

    sh_init(); /* init */

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
                    check_protected_cmd(cmd_map[i]);
                    ((int (*)())(cmd_map[i].func))();
                    break;
                case TYPE_ARG1:
                    check_arg_length(2);
                    check_protected_cmd(cmd_map[i]);
                    ((int (*)(char *, ...))(cmd_map[i].func))(argv[1]);
                    break;
                case TYPE_ARG2:
                    check_arg_length(3);
                    check_protected_cmd(cmd_map[i]);
                    ((int (*)(char *, ...))(cmd_map[i].func))(argv[1], argv[2]);
                    break;
                case 10:
                    check_arg_length(2);
                    check_protected_cmd(cmd_map[i]);
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
