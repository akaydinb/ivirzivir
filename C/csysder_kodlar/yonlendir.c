#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd;
    int i;

    if ((fd = open("test.txt", O_WRONLY|O_CREAT|O_TRUNC)) < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    close(1);
    dup(fd);

    for (i = 0; i < 10; ++i)
        printf("Test %d\n", i);

    close(fd);

    return 0;
}

