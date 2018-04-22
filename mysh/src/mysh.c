#include "mysh.h"

struct command commands[MAX_SIZE];
char *cwd;
char PROMPT[MAX_SIZE];
int num_commands;

void setup_shell() {
    //do some stuff to set up the shell
    //allocate cwd
    cwd = malloc(sizeof(char) * PATH_MAX);
    if(getcwd(cwd, PATH_MAX) == NULL) {
        perror("Error with getcwd");
    }  
    strcpy(PROMPT, "mysh:");
    strcat(PROMPT, cwd);
    strncat(PROMPT, "> ", 2);
}

void tokenize_commands(char *command_string) {
    char *tok, *toktok, *str1, *str2, *p1, *p2;
    int i = 0, j = 0, k = 0, c;
    int string_len;

    char copy_str[MAX_LN_SZ];
    /* The string passed might have the format >file or <file for redirection,
       and we want a space between those tokens, so we're going to loop through
       the array and build a new array with spaces if they aren't there. */
    int ch_flag = 0; //Flag if something was changed
    for(i = 0; command_string[i] != '\0'; i++) {
        ch_flag = 0;
        c = command_string[i];
        if(c == '<') {
            copy_str[k++] = ' ';
            copy_str[k++] = '<';
            copy_str[k++] = ' ';
            ch_flag = 1;
        }
        else if(c == '>') {
            copy_str[k++] = ' ';
            copy_str[k++] = '>';
            copy_str[k++] = ' ';
            ch_flag = 1;
        }
        if(ch_flag == 0) {
            copy_str[k++] = c;
        }
    }
    copy_str[k++] = '\0';
    trim(copy_str);
    /* DEBUG: Write original command_string array and new copy_string to stdout */
    // printf("%s\n", command_string);
    // printf("%s\n", copy_str);

    strncpy(command_string, copy_str, strlen(copy_str) + 1);

    /* The string that we have now is only a string, so we want to iterate
       through it and tokenize it into the array of command structs including
       a flag whether the command needs redirection or not and the file name
       if it does need redirection. We use pipes to separate simple commands
       and spaces to separate command tokens. */
    //This for loop initializes str1 to command_string and then for each subsequent iteration of the loop
    //it will set str1 to NULL. This is for strtok which expects a string at first
    //and then NULL for every subsequent call
    for(str1 = command_string; ; str1 = NULL) {
        k = 0;
        commands[j].redirIn = 0;
        commands[j].redirOut = 0;
        if((tok = strtok_r(str1, "|", &p1)) == NULL) {
            break;
        }
        for(str2 = tok; ; str2 = NULL) {
            if((toktok = strtok_r(str2, " ", &p2)) != NULL) {
                if(strcmp(toktok, "<") == 0) { //Found input redirection in curr command
                    commands[j].redirIn = 1; //Set input redirection flag to true
                    str2 = NULL; //strtok_r expects NULL for str2 after first call
                    toktok = strtok_r(str2, " ", &p2); //Next token should be infile
                    strncpy(commands[j].inFile, toktok, strlen(toktok));
                }
                else if(strcmp(toktok, ">") == 0) {
                    commands[j].redirOut = 1;
                    str2 = NULL;
                    toktok = strtok_r(str2, " ", &p2);
                    strncpy(commands[j].outFile, toktok, strlen(toktok));
                }
                else {
                    commands[j].tokens[k++] = toktok;
                }
            }
            else { break; }
        }
        commands[j].tokens[k] = NULL;
        commands[j].argc = k;
        j++;
        
    }
    num_commands = j; //Set the global variable num_commands to the number of commands parsed
}

void exec_commands() {
    //Iterate through commands doing redirection and piping as necessary
    if(num_commands == 1) {
        if(strcmp(commands[0].tokens[0], "mypwd") == 0 ||
          strcmp(commands[0].tokens[0], "pwd") == 0) {
            mypwd();
            return;
        }
        else if(strcmp(commands[0].tokens[0], "mycd") == 0 ||
                strcmp(commands[0].tokens[0], "cd") == 0) {
            //Call mycd for cd as well because I keep accidnetally typing cd instead of mycd
            mycd(commands[0].tokens);
            return;
        }
    }
    //Here we can begin to iterate through commands and set up pipes/execute them
    int lfd[2], rfd[2];
    int infd, outfd;
    int status;
    int i;
    pipe(lfd);
    for(i = 0; i < num_commands; i++) {
        if(num_commands > 1) {
            pipe(rfd);
        }
        pid_t pid = fork();
        if(pid == 0) { //Child
            if(commands[i].redirIn) {
                infd = open(commands[i].inFile, O_RDONLY);
                if(infd < 0) {
                    perror("Error opening infile.");
                    exit(1);
                }
                else {
                    dup2(infd, STDIN_FD);
                    close(infd);
                }
            }
            if(num_commands > 1) {
                if(i == 0) { //first pipe
                    dup2(rfd[PIPE_WRITE], STDOUT_FD);
                    close(rfd[PIPE_READ]);
                    close(rfd[PIPE_WRITE]);
                }
                else if(i == (num_commands - 1)) { //last pipe
                    dup2(lfd[PIPE_READ], STDIN_FD);
                    close(lfd[PIPE_READ]);
                    close(lfd[PIPE_WRITE]);
                }
                else {
                    dup2(lfd[PIPE_READ], STDIN_FD);
                    close(lfd[PIPE_READ]);
                    close(lfd[PIPE_WRITE]);
                    dup2(rfd[PIPE_WRITE], STDOUT_FD);
                    close(rfd[PIPE_READ]);
                    close(rfd[PIPE_WRITE]);
                }
            }
            if(commands[i].redirOut) {
                outfd = open(commands[i].outFile, O_WRONLY | O_CREAT | O_TRUNC,
                                                  S_IRUSR | S_IWUSR);
                if(outfd < 0) {
                    perror("Error creating/opening outfile");
                    exit(1);
                }
                else {
                    dup2(outfd, STDOUT_FD);
                    close(outfd);
                }
            }
            if(execvp(commands[i].tokens[0], commands[i].tokens) < 0) {
                fprintf(stderr, "Error execing command %s\n", commands[i].tokens[0]);
                exit(1);
            }
        }
        else if(pid > 0) {
            if(i > 0) {
                close(lfd[PIPE_READ]); //Close unnecessary left pipe
                close(lfd[PIPE_WRITE]);
            }
            lfd[PIPE_READ] = rfd[PIPE_READ]; // Save current right pipe as left pipe
            lfd[PIPE_WRITE] = rfd[PIPE_WRITE];
            wait(&status);
        }
    }
}

int main(int argc, char **argv) {
    char in_line[MAX_LN_SZ];
    setup_shell();
    while(1) {
        //Main loop
        printf("%s", PROMPT);
        in_line[0] = 0;
        fgets(in_line, MAX_LN_SZ, stdin); //get line from input
        in_line[strlen(in_line) - 1] = '\0'; //Get rid of trailing newline
        //Check to ensure line isn't blank and doesn't lead with spaces
        if(in_line[0] == ' ') {
            continue;
        }
        if(strcmp(in_line, "exit") == 0) {
            break;
        }
        tokenize_commands(in_line);
        exec_commands();
    }
    free(cwd);
    exit(0);
}