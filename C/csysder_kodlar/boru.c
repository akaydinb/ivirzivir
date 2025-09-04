#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>

void write_pipe(int fd);
void read_pipe(int fd);
void err_sys(const char *msg);

int main(int argc, char *argv[])
{
    int fdp[2];
    int pid;

    if (pipe(fdp) < 0) 
        err_sys("pipe");

    if ((pid = fork()) < 0) 
        err_sys("fork");

    if (pid) {    /* ust proses yazma yapacak */
        close(fdp[0]);
        write_pipe(fdp[1]);
        close(fdp[1]);
        if (wait(NULL) < 0)    err_sys("wait");
    }
    else {        /* alt proses okuma yapacak */
        close(fdp[1]);
        read_pipe(fdp[0]);
        close(fdp[0]);
    }

    //if (wait(NULL) < 0)    err_sys("wait");

    return 0;
}

void write_pipe(int fd)
{
    int i;

    for (i = 0; i < 100; ++i)
        if (write(fd, &i, sizeof(int)) < 0) 
            err_sys("write");
}

void read_pipe(int fd)
{
    int val;
    int result;

    while ((result = read(fd, &val, sizeof(int))) > 0)
        printf("%d\n", val);

    if (result < 0)
        err_sys("read");
}

void err_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


