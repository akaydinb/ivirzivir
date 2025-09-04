/* dirwalk1.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define MAX_PATH_LEN        2048

void err_sys(const char *msg)
{
    perror(msg);
    fflush(stdout);
    exit(EXIT_FAILURE);
}

void dirwalk(const char *dirpath)
{
    DIR *dir;
    struct dirent *ent;
    struct stat finfo;
    char path[MAX_PATH_LEN];
    static int level;

    if ((dir = opendir(dirpath)) == NULL) {
        perror("opendir");
        return;
    }

    while (errno = 0, (ent = readdir(dir)) != NULL) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;
        snprintf(path, MAX_PATH_LEN, "%s/%s", dirpath, ent->d_name);
        if (lstat(path, &finfo) < 0) 
            err_sys("lstat"); 
        printf("%*s%s\n", level * 4, "", ent->d_name);
        if (S_ISDIR(finfo.st_mode)) {
            ++level;
            dirwalk(path);
            --level;
        }
    }
    if (errno) 
        perror("readdir");
    
    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments!\nUsage: dirwalk1 <path>\n");
        exit(EXIT_FAILURE);
    }
    
    dirwalk(argv[1]);    
        
    return 0;
}

