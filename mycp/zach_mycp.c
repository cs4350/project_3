#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char*argv[])
{
  FILE *fp;
  FILE *fp2;
 
  if(access(argv[2], F_OK) != 0)
  {
    printf("mycp: cannot stat ‘");
    printf("%s", argv[1]);
    printf("’: No such file or directory\n");
    return 1;
  }
  
  fp = fopen(argv[1], "w");
  fp2 = fopen(argv[2], "r");
  char c;

  while(!feof(fp2))
  { 
    c = fgetc(fp2);
    fputc(c, fp);
  }
   
  fclose(fp);
  fclose(fp2);

  return 0;
}
