#include "shell.h"
#define LENCD 128

int mycd(int argc, char *arglist[])
{
    if (argc > 2)
    {
        printf("cd: too many arguments\n");
        return 1;
    }
    // cd: too many arguments

    char home[LENCD];
    struct passwd *pass;
    pass = getpwuid(getuid());

    sprintf(home, "/home/%s", pass->pw_name);

    if (arglist[1] == NULL)
    {
        arglist[1] = (char *)malloc(sizeof(home));
        strcpy(arglist[1], home);
    }
    // 回到主目录

    int sta = chdir(arglist[1]);
    if (sta < 0) // cannot find the file
    {
        printf("\e[31;1mcd: No such file or directory: %s\n\n\e[0m", arglist[1]);
        return 1;
    }
    printf("\n");
}