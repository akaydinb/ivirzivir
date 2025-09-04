#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h> 

#define BUFSIZE 32

void do_parent_proc(int fdp, int fdf);
void do_child_proc(int fdp);
void err_sys(const char *msg);

int main(int argc, char *argv[])
{
    int fdf, fdps[2];
    pid_t pidChild;

    if (argc != 2) {
        fprintf(stderr, "eksik ya da fazla sayida arguman girildi! ");
        exit(EXIT_FAILURE);
    }

    if ((fdf = open(argv[1], O_RDONLY)) < 0) 
        err_sys("open");

    if (pipe(fdps)< 0) 
        err_sys("pipe");

    if ((pidChild = fork()) < 0) 
        err_sys("fork");

    if (pidChild > 0) {
        close(fdps[0]);
        do_parent_proc(fdps[1], fdf);
        close(fdps[1]);
        wait(NULL);
    }
    else {
        close(fdps[1]);
        do_child_proc(fdps[0]);
        close(fdps[0]);
    }

    close(fdf);

    return 0;
}

void do_parent_proc(int fdp, int fdf)
{
    char buf[BUFSIZE];
    ssize_t n;

    while ((n = read(fdf, buf, BUFSIZE)) > 0)
        if (write(fdp, buf, n) < 0) 
            err_sys("pipe");
    if (n < 0) 
        err_sys("read");
}

void do_child_proc(int fdp)
{
    ssize_t n;
    char buf[BUFSIZE];

    while ((n = read(fdp, buf, BUFSIZE)) > 0) 
        if (write(STDOUT_FILENO, buf, n) != n) 
            err_sys("write");
    if (n < 0) 
        err_sys("read");
}

void err_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

