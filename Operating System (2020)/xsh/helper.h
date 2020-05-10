#ifndef _XSH_HELPER_H_
#define _XSH_HELPER_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFSIZE 0xFFFF

#define EX_SUCCESS 0x00
#define EX_BUILTIN 0x01
#define EX_INVALID 0x02

#define FIRST_CMD 0x01
#define MIDDLE_CMD 0x02
#define LAST_CMD 0x03

#define READ_END 0x00
#define WRITE_END 0x01

#define BUILTIN_CMD_CD "cd"
#define BUILTIN_CMD_EXEC "exec"
#define BUILTIN_CMD_EXIT "exit"
#define BUILTIN_CMD_HELP "help"

#define C_RED "\033[0;31m"
#define C_GREEN "\033[0;32m"
#define C_YELLOW "\033[0;33m"
#define C_BLUE "\033[0;34m"
#define C_MAGENTA "\033[0;35m"
#define C_CYAN "\033[0;36m"
#define C_RESET "\033[0m"

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

extern int last_retval;

extern int fmode;
extern char *fredir;

extern char buf[BUFSIZE];
extern char *arguments[BUFSIZE];
extern char **commands[BUFSIZE];

static bool isprintable(char *);

static int parse_arguments(char *, char **);

static const char *parse_dir(const char *);

size_t __lenof__(void **);

void show_prompt(void);

int read_line(FILE *);

int execute(char **, int, int *, int *);

int execute_builtin(char **);

int parse_commands(void);

#endif
