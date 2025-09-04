#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void err_sys(const char *msg);

int main(void)
{
    char *env[] = {"name=Kaan Aslan", "no=100", NULL};
    char *args[] = {"dispenv", NULL};

    pid_t pid;

    if ((pid = fork()) < 0)
        err_sys("fork");
    if (pid == 0) 
        if (execve("dispenv", args, env) < 0)
            err_sys("execve");
     
    if (waitpid(pid, NULL, 0) < 0)
        err_sys("waitpid");

    return 0;
}

void err_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

