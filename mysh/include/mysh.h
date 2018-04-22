#ifndef _MYSH_H
#define _MYSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
#define STDIN_FD 0
#define STDOUT_FD 1
#define MAX_SIZE 256
#define MAX_LN_SZ 4096

struct command {
    char *tokens[MAX_SIZE];
    int redirIn;
    int redirOut;
    char inFile[MAX_SIZE];
    char outFile[MAX_SIZE];
    int argc;
};

extern struct command commands[MAX_SIZE];
extern char PROMPT[MAX_SIZE]; //prompt shown to user
extern char *cwd; //current working directory, global for shell

#endif