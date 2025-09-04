#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void err_sys(const char *msg);

pid_t exec_pipe(const char *filename, const int fdps[2], int rw)
{
    pid_t pid;

    if ((pid = fork()) < 0) 
        err_sys("fork");

    if (pid == 0) {
        if (dup2(fdps[rw], rw) == -1) 
            err_sys("dup2");

        close(fdps[0]);
        close(fdps[1]);

        if (execlp(filename, filename, (char *) NULL) == -1) 
            err_sys("execlp");
    }

    return pid;
}

int main(int argc, char *argv[])
{
    pid_t pid1, pid2;
    int fdps[2];

    if (argc != 3) {
        fprintf(stderr, "kullanim: %s <prog1> <prog2> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (pipe(fdps) == -1) 
        err_sys("pipe");

    pid1 = exec_pipe(argv[1], fdps, 1);
    pid2 = exec_pipe(argv[2], fdps, 0);

    close(fdps[0]);
    close(fdps[1]);

    if (waitpid(pid1, NULL, 0) < 0)
        err_sys("waitpid");

    if (waitpid(pid2, NULL, 0) < 0)
        err_sys("waitpid");

    return 0;
}

void err_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

