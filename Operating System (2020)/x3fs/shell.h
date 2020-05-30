#ifndef _SHELL_H
#define _SHELL_H

#include <stdio.h>
#include <stdlib.h>

#include "fs.h"

#define check_arg_length(x)                                \
    do                                                     \
    {                                                      \
        if (argc < (x))                                    \
        {                                                  \
            fprintf(stderr, "shell: too few arguments\n"); \
            goto out_loop;                                 \
        }                                                  \
    } while (0)

#define check_protected_cmd(x)                                    \
    do                                                            \
    {                                                             \
        if (!mounted && (x).protected)                            \
        {                                                         \
            fprintf(stderr, "shell: mount before %s\n", (x).cmd); \
            goto out_loop;                                        \
        }                                                         \
    } while (0)

#define exit_shell()          \
    do                        \
    {                         \
        if (mounted)          \
        {                     \
            fs_writeto(NULL); \
        }                     \
        exit(EXIT_SUCCESS);   \
    } while (0)

#define BUFSIZE 0xFF

extern char buf[BUFSIZE];
extern int argc;
extern char *argv[BUFSIZE - 1];
extern bool mounted;

void sh_help();
void sh_pwd();
void sh_umount();

void sh_cat(const char *);
void sh_mount(const char *);

void sh_cpi(const char *, const char *);
void sh_cpo(const char *, const char *);

void sh_init();
void sh_exit();
void sh_promot();

struct cmd_t
{
    char *cmd;
    char *desc;
    void (*func)();
    bool protected;
#define TYPE_ARG0 0
#define TYPE_ARG1 1
#define TYPE_ARG2 2
#define TYPE_EXIT -1
    int type;
};

struct cmd_t cmd_map[] = {
    {"help", "show help message", (void (*)())sh_help, false, TYPE_ARG0},
    {"mount", "mount file system", (void (*)())sh_mount, false, TYPE_ARG1},
    {"umount", "unmount file system", (void (*)())sh_umount, false, TYPE_ARG0},
    {"mkdir", "make directory", (void (*)())fs_mkdir, true, TYPE_ARG1},
    {"rmdir", "remove directory", (void (*)())fs_rmdir, true, TYPE_ARG1},
    {"ls", "list directory contentss", (void (*)())fs_ls, true, TYPE_ARG0},
    {"cd", "change directory", (void (*)())fs_cd, true, TYPE_ARG1},
    {"pwd", "return working directory name", (void (*)())sh_pwd, true, TYPE_ARG0},
    {"touch", "create file", (void (*)())fs_create, true, TYPE_ARG1},
    {"cat", "concatenate and print files", (void (*)())sh_cat, true, TYPE_ARG1},
    {"cpi", "copy from local to x3fs", (void (*)())sh_cpi, true, TYPE_ARG2},
    {"cpo", "copy from x3fs to local", (void (*)())sh_cpo, true, TYPE_ARG2},
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
