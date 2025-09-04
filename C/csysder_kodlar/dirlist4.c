#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

void err_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *ent;
    struct stat finfo;
    int result;
    
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments!..\n");
        exit(EXIT_FAILURE);
    }

    if ((dir = opendir(argv[1])) == NULL) 
        err_sys("opendir");

    if (chdir(argv[1]) < 0) 
        err_sys("chdir");

    while (errno = 0, (ent = readdir(dir)) != NULL) {
        if (stat(ent->d_name, &finfo) < 0) 
            err_sys("stat");

        printf("%-30s", ent->d_name);
        if (S_ISDIR(finfo.st_mode))
            printf("<DIR>\n");
        else
            printf("%lu\n", finfo.st_size);
    }

    if (errno) 
        err_sys("readdir");
        
    closedir(dir);
    
    return 0;
}

