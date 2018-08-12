#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

void ls(char *path)
{
    DIR *dp = NULL;
    struct dirent *dptr = NULL;
      // Open the directory stream
      if(NULL == (dp = opendir(path)) )
      {
          printf("\n Cannot open Input directory [%s]\n",path);

      }
      else
      {
        // Read the directory contents
        while(NULL != (dptr = readdir(dp)) )
        {
             char name[255];
             strncpy(name, dptr -> d_name, 254);
             name[254] = '\0';
            if(name[0] != '.')
            {
               printf("%s \n", name);
            }
           // printf(" %s \n",dptr->d_name);
        }
        // Close the directory stream
        closedir(dp);
      }

};

void ls_l(char *f)
{
    struct stat fileStat;
   // printf("%s \n", f);
    char d[1024];
    if(getcwd(d, sizeof(d)) != NULL)
   {
    strcat(d, "/");
    strcat(d, f);
   // printf(" %s ", d);
    if(stat(d, &fileStat) < 0)
    {
        printf("Cannot access file or directory \n");
        return;
    }
    struct group *grp;
    struct passwd *pwd;
    grp = getgrgid(fileStat.st_gid);
    pwd = getpwuid(fileStat.st_uid);
    //Print permissions and all information
    //printf("%s ",f);
    //printf("---------------------------\n");
    //printf("%ld bytes ",fileStat.st_size);
    //printf(" %lu ",fileStat.st_nlink);
    //printf(" %lu ",fileStat.st_ino);

   //printf("File Permissions:");
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf(" %s", grp ->gr_name);
    printf(" %s",pwd->pw_name); 
    printf(" %lu ", fileStat.st_nlink);
    printf("%ld bytes ", fileStat.st_size);
    //printf("%lu", fileStat.st_ino);
    printf(" %s \n", f);
}
};

void ls_lall(char *path)
{
        DIR *dp = NULL;
       struct dirent *dptr = NULL;
       char save[256]; 
       strcpy(save, path);
      // Open the directory stream
      if(NULL == (dp = opendir(path)) )
      {
          printf("\n Cannot open Input directory [%s]\n",path);

      }
      else
      {
        // Read the directory contents
        while(NULL != (dptr = readdir(dp)) )
        {
             char name[255];
             strncpy(name, dptr -> d_name, 254);
             name[254] = '\0';
             
            if(name[0] != '.')
            {
                strcat(path, "/");
                ls_l(strcat(path, name));
                strcpy(path, save);
            }
           // printf(" %s \n",dptr->d_name);
        }
        // Close the directory stream
        closedir(dp);
      }

};

void ls_path(char *path)
{
    char wd[1024];
    if (getcwd(wd, sizeof(wd)) != NULL)
    {
       if(path[0] != '/')
        {
          strcat(wd, "/");
          printf(" %s \n", wd);
          strcat(wd, path);
         //printf("The directory passed was %s \n", wd);
         ls(wd);

        }
        else
        {
          printf("Can't access %s \n", path);
        }
    }
    else
    {
        printf("Oh shit..\n");
    }
};

int main (int argc, char *argv[])
{
    if (argc == 1)
    {
       char cwd[1024];
       if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            ls(cwd);
        }
        return 1;
    }
    if (strcmp(argv[1], "-l") == 0)
    {
       if (argc == 2)
       {
         char cwd[1024];
         if (getcwd(cwd, sizeof(cwd)) !=NULL)
         {
          ls_lall(cwd);
         //printf("-l was passed but with no directory\n");
         }
         return 2;
       }
       else if (argc == 3)
       {
          
         ls_lall(argv[2]);
         //printf("Function ls_l will figure it out\n");
         return 3;
       }
    }
    else
    {
       if(strstr(argv[1], "/") != NULL)
       {
           ls_path(argv[1]);
          // printf("LS_path will handle this \n");
           return 4;
       }
       else if(strstr(argv[1], "-") != NULL)
       {
         printf("There is no option for %s \n",argv[1]);
         return 5;
       }
       else
       {
          ls_path(strcat(argv[1], "/"));
          return 7;
       }
    }
    return 0;
}
