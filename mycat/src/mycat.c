#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#define MAX_FILE_NAME_CHARS 128

int input(char *s,int length);

int main(int argc, char *argv[])
{
 FILE *fp;
 char file_name[MAX_FILE_NAME_CHARS],
      ch;       
 int i;
 struct stat buf;

 
 //if there is no arguments for mycat command
 if(argc < 2)
 {
    
    char *buffer;
    size_t bufsize = 1024;
    size_t characters;

    buffer = (char *)malloc(bufsize * sizeof(char));

    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }
    
    while((characters = getline(&buffer,&bufsize,stdin)) != -1)
    {
      fwrite(buffer,characters,1,stdout);
    }
    return(0);
 }

 printf("\n");

 //for-loop for iterating through i amount of
 //files since cat can handle more than one file
 for(i = 1; i < argc; i++)
 {
    //for each file, strncpy to file_name
    strncpy(file_name, argv[i], MAX_FILE_NAME_CHARS);

    if(stat(file_name, &buf) == -1)
    {
       perror("Cannot stat the file");
       return 1;
    }

    if(S_ISDIR(buf.st_mode)){
      printf("mycat: %s: is a directory\n\n", file_name);
      continue;
    }
   
    //open file
    fp = fopen(file_name, "r");
    if(fp == NULL) 
    {
       printf("%s: No such file\n", file_name);
       return 0;
    }

    //read char by char and display to output
    while((ch = fgetc(fp)) != EOF){
         printf("%c",ch);
    }

    printf("\n");
  
    //close file
    fclose(fp); 
 } 

 return 0;
}
