/* dispenv.c */

#include <stdio.h>

extern char **environ;

int main(void)
{
    int i;

    for (i = 0; environ[i] != NULL; ++i)
        puts(environ[i]);

    return 0;
}

