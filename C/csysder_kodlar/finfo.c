#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    struct stat finfo;

    if (argc != 2) {
        fprintf(stderr, "Yanlış sayida komut satiri argumani girildi!..");
        exit(EXIT_FAILURE);
    }
        
    if (stat(argv[1], &finfo) < 0) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("Dugum numarasi: %lu\n", finfo.st_ino);
    /* ... */

    return 0;
}

