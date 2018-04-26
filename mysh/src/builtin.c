#include "mysh.h"

void mypwd()
{
  char path[PATH_MAX];
  if(getcwd(path, sizeof(path)) == NULL)
    perror("getcwd() error");
  else
    printf("%s\n", path);
}

/* Takes an array that contains the tokenized command input string */
void mycd(const char **argv) {
  // if(strcmp(argv[0], "mycd") != 0) {
  //   return;
  // }
  //Only use the first argument
  if(argv[1] == NULL) {
    return;
  }
  char *cdir = argv[1];
  if(strcmp(cdir, "~") == 0) {
    cdir = getenv("HOME");
  }
  //Set working directory
  if(chdir(cdir) == -1) {
    perror("Couldn't change directory.");
  }
  else {
    if(getcwd(cwd, PATH_MAX) == NULL) {
        perror("Error with getcwd");
    }  
    strcpy(PROMPT, "mysh:");
    strcat(PROMPT, cwd);
    strncat(PROMPT, "> ", 2);
  }
}

void trim(char *s) {
    char *p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(*p && isspace(*p)) ++p, --l;

    memmove(s, p, l + 1);
}   