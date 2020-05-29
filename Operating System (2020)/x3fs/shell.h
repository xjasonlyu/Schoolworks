#ifndef _SHELL_H
#define _SHELL_H

#include <stdio.h>
#include <stdlib.h>

#include "fs.h"

#define check_arg_length(x)                   \
    do                                        \
    {                                         \
        if (argc < (x))                       \
        {                                     \
            puts("shell: too few arguments"); \
            goto out_loop;                    \
        }                                     \
    } while (0)

#define check_protexted_cmd(x)                           \
    do                                                   \
    {                                                    \
        if (!mounted && (x).protected)                   \
        {                                                \
            printf("shell: mount before %s\n", (x).cmd); \
            goto out_loop;                               \
        }                                                \
    } while (0)

#define BUFSIZE 0xFF

extern char buf[BUFSIZE];
extern int argc;
extern char *argv[BUFSIZE - 1];
extern bool mounted;

void sh_promot();
void sh_help();
void sh_pwd();
void sh_mount(const char *);
void sh_umount();
void sh_exit();

struct cmd_t
{
    char *cmd;
    char *desc;
    void (*func)();
    bool protected;
#define TYPE_ARG0 0
#define TYPE_ARG1 1
#define TYPE_EXIT -1
    int type;
};

struct cmd_t cmd_map[] = {
    {"help", "show help message", (void (*)())sh_help, false, TYPE_ARG0},
    {"mount", "mount disk file", (void (*)())sh_mount, false, TYPE_ARG1},
    {"umount", "umount disk file", (void (*)())sh_umount, false, TYPE_ARG0},
    {"mkdir", "make directory", (void (*)())fs_mkdir, true, TYPE_ARG1},
    {"rmdir", "remove directory", (void (*)())fs_rmdir, true, TYPE_ARG1},
    {"ls", "list files", (void (*)())fs_ls, true, 0},
    {"cd", "change directory", (void (*)())fs_cd, true, TYPE_ARG1},
    {"touch", "create file", (void (*)())fs_create, true, TYPE_ARG1},
    // {"create", "", (void (*)())fs_create, 1},
    {"open", "open file", (void (*)())fs_open, true, TYPE_ARG1},
    {"close", "close file via fd", (void (*)())fs_close, true, 10},
    {"write", "write file via fd", (void (*)())fs_write, true, 10},
    {"read", "read file via fd", (void (*)())fs_read, true, 10},
    {"rm", "remove file", (void (*)())fs_rm, true, TYPE_ARG1},
    {"exit", "exit this shell", (void (*)())sh_exit, false, TYPE_EXIT},
    {NULL, NULL, NULL, false, 0}};
#define CMD_MAP_LEN (sizeof(cmd_map) / sizeof(struct cmd_t))

#endif