#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ftw.h>
#include <dirent.h>

static int do_recursive_copy(const char *file_path, 
                             const struct stat *st_buf, 
                             int type_flags,
                             struct FTW *ftw_buf) 
{
    if(type_flags == FTW_NS) {
        fprintf(stderr, "Error reading file at %s: Permission denied.\n", file_path);
    }
    else if(type_flags == FTW_D) {
        printf("Found directory at %s\n", file_path);
    }
    else if(type_flags == FTW_F) {
        printf("Found regular file at %s\n", file_path);
    }
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
    if(RECUR_CP) {
        printf("Directories will be copied recursively.\n");
    }
    if(stat(argv[argsind], &s_stat) == -1) {
        perror("Can't stat source.\n");
        exit(1);
    }
    if(stat(argv[argsind + 1], &d_stat) == -1) {
        perror("Can't stat destination.\n"); //Program will fail if not given destination
        exit(1);
    }
    
    mode_t s_mode = (s_stat.st_mode & S_IFMT);
    mode_t d_mode = (d_stat.st_mode & S_IFMT);

    //Test source to determine if file or directory
    if(s_mode == S_IFDIR) {
        int ret;
        int flags = FTW_PHYS | FTW_MOUNT;
        int fd_limit = 5;
        printf("Source is a directory.\n");
        ret = nftw(argv[argsind], do_recursive_copy, fd_limit, flags);
    }
    else if(s_mode == S_IFREG) {
        printf("Source is a regular file.\n");
    }
    else {
        perror("Source isn't a directory or regular file.\n");
    }
    
    //Test destination
    if(d_mode == S_IFDIR) {
        printf("Dest is a directory.\n");
    }
    else if(d_mode == S_IFREG) {
        printf("Dest is a regular file.\n");
    }
    else {
        perror("Dest isn't a directory or a regular file.\n");
    }

    exit(0);
}