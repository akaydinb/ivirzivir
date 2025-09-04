#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void err_sys(const char *msg)
{
    perror(msg);
    fflush(stdout);
    exit(EXIT_FAILURE);
}

int main(void)
{
    pid_t pidChild;

    if ((pidChild = fork()) < 0) 
        err_sys("fork");

    if (pidChild == 0)              /* Alt proses */
        exit(EXIT_SUCCESS);
    else {                          /* Ust proses */
        printf("Ust prosesi sonlandirmak icin ENTER tusuna basiniz...\n");
        getchar();                    
    }
    
    return 0;
}

