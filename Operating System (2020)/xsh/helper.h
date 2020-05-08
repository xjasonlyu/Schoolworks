#ifndef _XSH_HELPER_H_
#define _XSH_HELPER_H_

#include <stdlib.h>
#include <fcntl.h>

#define EX_SUCCESS 0x00
#define EX_BUILTIN 0x01
#define EX_INVALID 0x02

#define FIRST_CMD 0x01
#define MIDDLE_CMD 0x02
#define LAST_CMD 0x03

#define READ_END 0x00
#define WRITE_END 0x01

#define RE_WRITE_MODE (O_WRONLY | O_CREAT | O_TRUNC)
#define RE_APPEND_MODE (O_WRONLY | O_CREAT | O_APPEND)
#define RE_DEFAULT_MODE RE_WRITE_MODE

#define die(code, msg) \
    do                 \
    {                  \
        puts((msg));   \
        _exit((code)); \
    } while (0)

#define lenof(p) __lenof__((void **)(p))

size_t __lenof__(void **);

void show_promot(void);

void waitn(int);

int read_line(char *, size_t);

int excute(char **, int, int *, int *);

int excute_builtin(char **);

int parse_arguments(char *, char **);

int parse_commands(char *, char **, char ***, char **, int *);

#endif