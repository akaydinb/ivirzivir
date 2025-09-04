#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

/* Symbolic Constants */

#define PROMPT "CSD"
#define MAX_CMD_LEN            256
#define MAX_CMD_PARAM          20
#define MAX_PATH               4096

/* Type Declarations */

typedef struct tagCMD {
     char *pCmdText;
     void (*proc) (void);
} CMD;

/* Function Prototypes */

void err_sys(const char *msg);
void put_prompt(void);
void parse_cmd_line(void);
void change_dir(void);
void list_dir(void);

/* Global Data Definitions */

char g_cmdText[MAX_CMD_LEN];
CMD g_cmd[] = {
    {"cd", change_dir}, {"dir", list_dir}, {NULL, NULL}
};
char *g_params[MAX_CMD_PARAM];
int g_nParams;
char g_cwd[MAX_PATH];

/* Function Definitions */

int main(void)
{
    char *pStr;
    int i;

    for (;;) {
        put_prompt();
        fgets(g_cmdText, MAX_CMD_PARAM, stdin);
        if ((pStr = strchr(g_cmdText, '\n')) != NULL)
            *pStr = '\0';
        parse_cmd_line();
        if (!g_nParams)
            continue;
        if (!strcmp(g_params[0], "exit"))
            break;
        for (i = 0; g_cmd[i].pCmdText != NULL; ++i)
            if (!strcmp(g_params[0], g_cmd[i].pCmdText)) {
                g_cmd[i].proc();
                break;
            }
        if (g_cmd[i].pCmdText == NULL)
            fprintf(stderr, "invalid command: %s\n", g_params[0]);
     }

     return 0;
}

void err_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void put_prompt(void)
{
    if (!getcwd(g_cwd, MAX_PATH)) 
        err_sys("getcwd");
    printf("%s:%s>", PROMPT, g_cwd);
}

void parse_cmd_line(void)
{
    char *pStr;
    int i = 0;

    for (pStr = strtok(g_cmdText, " \t"); pStr != NULL; pStr = strtok(NULL, " \t"))
        g_params[i++] = pStr;

    g_nParams = i;
}

void change_dir(void)
{
    if (g_nParams != 2) {
        fprintf(stderr, "wrong number of arguments!..\n");
        return;
    }

    if (chdir(g_params[1]) < 0) 
        perror("chdir");
}

void list_dir(void)
{
    DIR *dir;
    struct dirent *ent;
    struct stat finfo;
    char pathfile[MAX_PATH], *pathdir;
    unsigned long tsize = 0;
    int nfiles = 0;

    if (g_nParams > 2) {
        fprintf(stderr, "wrong number of arguments!..\n");
        return;
    }

    pathdir = g_nParams == 1 ? g_cwd : g_params[1];

    if ((dir = opendir(pathdir)) == NULL) {
        perror("opendir");
        return;
    }

    printf("\n");
    while (errno = 0, (ent = readdir(dir)) != NULL) {
        sprintf(pathfile, "%s/%s", pathdir, ent->d_name);
        if (lstat(pathfile, &finfo) < 0) 
            err_sys("stat");

        printf("%-30s", ent->d_name);
        if (S_ISDIR(finfo.st_mode))
            printf("<DIR>\n");
        else {
            printf("%lu\n", finfo.st_size);
            ++nfiles;
            tsize += finfo.st_size;
        }
     }

    if (errno) 
        err_sys("readdir");

    printf("\t%d file(s)\t\t%lu\n\n", nfiles, tsize);

    closedir(dir);
}

