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
        errorf("parse argument failed: %s", strerror(-retval));
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
    // printf("(%s)> ", mounted ? get_abspath(cur_dir) : "NULL");
    printf("%s➜ " C_RESET, mounted ? C_GREEN : C_RED);
}

void sh_help()
{
    puts("Commands:");
    for (int i = 0; i < CMD_MAP_LEN - 1; ++i)
    {
        printf("%s)%9s\t%s\n", (cmd_map[i].protected ? C_RED "*" C_RESET : C_GREEN "+" C_RESET),
               cmd_map[i].cmd, cmd_map[i].desc);
    }
    return;
}

void sh_clear()
{
    const char magic_ansi[] = "\e[1;1H\e[2J";
    write(STDOUT_FILENO, magic_ansi, sizeof(magic_ansi));
}

void sh_pwd()
{
    puts(get_abspath(cur_dir));
}

void sh_mount(const char *filename)
{
    if (mounted)
    {
        errorf("Current disk is in use");
        return;
    }
    if (access(filename, F_OK) < 0)
    {
        errorf("Disk file not found");
        return;
    }

    if (fs_loadfrom(filename) > 0)
        mounted = true;
}

void sh_umount()
{
    if (!mounted)
    {
        errorf("No disk mounted");
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
        // errorf( "cannot open %s", filename);
        return;
    }

    int n = 0;
    int size = ofs[_fd].fcb.size;
    char *b = malloc(BLOCK_SIZE);

    while (size > 0)
    {
        if ((n = fs_read(_fd, b, BLOCK_SIZE)) < 0)
        {
            // errorf( "read error");
            break;
        }
        if (write(STDOUT_FILENO, b, n) < 0)
        {
            break;
        }
        size -= BLOCK_SIZE;
    }

    free(b);
    fs_close(_fd);
}

void sh_append(const char *filename)
{
    int _fd = fs_open(filename);
    if (_fd < 0)
    {
        // errorf("cannot open %s", filename);
        return;
    }

    // seek to end
    fs_seek(_fd, 0, SEEK_END);

    int n = 0;
    char buffer[0x100] = {0};
    while ((n = read(STDIN_FILENO, buffer, 0xFF)) > 0)
    {
        if (fs_write(_fd, buffer, n) < 0)
        {
            break;
        }
    }

    fs_close(_fd);
}

void sh_cpi(const char *src, const char *dst)
{
    int src_fd = open(src, O_RDONLY);
    if (src_fd < 0)
    {
        errorf("cannot open %s", src);
        return;
    }

    if (fs_create(dst) < 0)
    {
        // errorf("cannot create %s", dst);
        close(src_fd);
        return;
    }

    int dst_fd = fs_open(dst);
    if (dst_fd < 0)
    {
        // errorf("cannot open %s", dst);
        close(src_fd);
        return;
    }

    void *b = malloc(BLOCK_SIZE);
    int n = 0;
    while ((n = read(src_fd, b, BLOCK_SIZE)) > 0)
    {
        if (fs_write(dst_fd, b, n) < 0)
        {
            break;
        }
    }

    free(b);
    close(src_fd);
    fs_close(dst_fd);
}

void sh_cpo(const char *src, const char *dst)
{
    if (access(dst, F_OK) != -1)
    {
        errorf("%s: File exists", dst);
        return;
    }

    int src_fd = fs_open(src);
    if (src_fd < 0)
    {
        // errorf("cannot open %s", src);
        return;
    }

    int dst_fd = open(dst, O_WRONLY | O_CREAT,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (dst_fd < 0)
    {
        errorf("cannot create %s", dst);
        fs_close(src_fd);
        return;
    }

    void *b = malloc(BLOCK_SIZE);
    int n = 0;
    while ((n = fs_read(src_fd, b, BLOCK_SIZE)) > 0)
    {
        if (write(dst_fd, b, n) < 0)
        {
            break;
        }
    }

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
            puts("command not found");
        }
    }

out:
    return retval;
}
