#define _XOPEN_SOURCE 700
#include <ftw.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>

char* dest_dir;

static int do_recursive_copy(const char *file_path, 
                             const struct stat *st_buf, 
                             int type_flags,
                             struct FTW *ftw_buf) 
{
    char* fullpath = malloc(sizeof(char)*PATH_MAX);
    strncpy(fullpath, dest_dir, strlen(dest_dir)+1);
    strncat(fullpath, "/\0",2);
    strncat(fullpath, file_path,strlen(file_path)+1);
    int rfd, wfd;
    
    if(type_flags == FTW_NS) {
        fprintf(stderr, "Error reading file at %s: Permission denied.\n", file_path);
    }
    else if(type_flags == FTW_D) {
        printf("Copying file %s\n", file_path);
        // printf("Directory %s will be made\n", fullpath);
        mkdir(fullpath, st_buf->st_mode);
    }
    else if(type_flags == FTW_F) {
        char buf[1024];
        int bytes_read;
        printf("Copying file %s\n", file_path);
        if((rfd = open(file_path, O_RDONLY)) == -1){
            perror("Can not open file");
            return -1;
        }
        if((wfd = open(fullpath, O_WRONLY|O_CREAT|O_TRUNC,st_buf->st_mode)) == -1){
            perror("Can not open file");
            return -1;
        }
        do{
            bytes_read = read(rfd, &buf, sizeof(buf));
            write(wfd, &buf, bytes_read);
        }while(bytes_read > 0);
        close(wfd);
        close(rfd);
    }
    free(fullpath);
    return 0;
}

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "Usage: %s [-r|-R] <source> <destination>\n", argv[0]);
        exit(1);
    }
    struct stat s_stat; //Stat struct for source
    struct stat d_stat; //Stat struct for desitnation
    int RECUR_CP = 0;
    int opt, argsind = 1;

    while((opt = getopt(argc, argv, "rR")) != -1) {
        switch(opt) {
            case 'R':
            case 'r':
                RECUR_CP = 1;
                break;
            default:
                break;
        }
    }
    argsind = optind;

    /* We will have several cases we need to cover:
        * Copy a source file to a dest file, open dest file with O_TRUNC
        * Copy a source file to a dest dir
        * Copy a source dir to a dest dir recursively and only recursively, produce error if no -r flag
           * We need to be sure to make the source directory and all the files/directories inside it in the dest dir,
             not just copy all the files/directories inside the source dir
        * Produce error on attempt to copy dir to file
    */
    if(stat(argv[argsind], &s_stat) == -1) {
        perror("Can't stat source.");
        exit(1);
    }
    if(stat(argv[argsind + 1], &d_stat) == -1) {
        if(RECUR_CP == 1){
            mkdir(argv[argsind+1],0775);
            if(stat(argv[argsind + 1], &d_stat)==-1){
                perror("Can't create directory");
                exit(1);
            }
        }
        else if((s_stat.st_mode & S_IFMT) == S_IFDIR){
            fprintf(stderr,"cp: omitting directory \'test\'\n");
            exit(1);
        }
        else{
            int wfd = open(argv[argsind+1], O_WRONLY|O_CREAT|O_TRUNC, 0664);
            if(stat(argv[argsind + 1], &d_stat)==-1){
                perror("Can't create file");
                exit(1);
            }
            close(wfd);
        }
    }
    
    mode_t s_mode = (s_stat.st_mode & S_IFMT);
    mode_t d_mode = (d_stat.st_mode & S_IFMT);

    //Test source to determine if file or directory
    if(s_mode == S_IFDIR) {
        if((d_mode == S_IFDIR) && (RECUR_CP==1)){
            int ret;
            int flags = FTW_PHYS | FTW_MOUNT;
            int fd_limit = 5;
            dest_dir = argv[argsind + 1];
            ret = nftw(argv[argsind], do_recursive_copy, fd_limit, flags);
        }
        else{
            fprintf(stderr,"ERROR\n");
            exit(1);
        }
    }
    else if(s_mode == S_IFREG) {
        if(d_mode == S_IFDIR){
            char fullpath[PATH_MAX];
            strncpy(fullpath, argv[argsind+1], strlen(argv[argsind+1])+1);
            strncat(fullpath, "/", 2);
            strncat(fullpath, argv[argsind],strlen(argv[argsind])+1);
            int rfd,wfd;
            if((rfd = open(argv[argsind], O_RDONLY))==-1){
                perror("Can not open file");
                exit(1);
            }
            if((wfd = open(fullpath, O_WRONLY|O_CREAT|O_TRUNC, s_stat.st_mode)) == -1){
                perror("Can not open file");
                exit(1);
            }
            printf("Copying %s\n", argv[argsind]);
            int bytes_read;
            char buf[1024];
            do{
                bytes_read = read(rfd, &buf, sizeof(buf));
                write(wfd, &buf, bytes_read);
            }while(bytes_read > 0);
            close(wfd);
            close(rfd);
        }
        else if(d_mode == S_IFREG){
            int rfd,wfd;
            if((rfd = open(argv[argsind], O_RDONLY))==-1){
                perror("Can not open file");
                exit(1);
            }
            if((wfd = open(argv[argsind+1], O_WRONLY|O_CREAT|O_TRUNC, d_stat.st_mode)) == -1){
                perror("Can not open file");
                exit(1);
            }
            printf("Copying %s\n", argv[argsind]);
            int bytes_read;
            char buf[1024];
            do{
                bytes_read = read(rfd, &buf, sizeof(buf));
                write(wfd, &buf, bytes_read);
            }while(bytes_read > 0);
            close(wfd);
            close(rfd);
        }
    }
    else {
        perror("Source isn't a directory or regular file.\n");
    }
    exit(0);
}