#include "shell.h"
#define PROMPT_SIZE 1024

char prompt_buffer[PROMPT_SIZE]; //prompt提示符

int find(char *str, char *ch) // 查询字符首次出现的位置，找到时返回索引，否则返回-1
{
    int len1 = strlen(str);
    int len2 = strlen(ch);
    int flag = 1;

    if (len1 < len2)
        return -1;
    for (int i = 0; i < len1 - len2 + 1; i++)
    {
        flag = 1;
        for (int j = 0; j < len2; j++)
        {
            if (str[i + j] != ch[j])
            {
                flag = 0;
                break;
            }
        }
        if (flag)
            return i;
    }
    return -1;
}

void Init()
{
    argc = 0;
    memset(arglist, 0x00, sizeof(arglist));
    // 命令列表初始化

    char pwd[LEN];
    char name[LEN];
    struct passwd *pass;

    gethostname(name, sizeof(name) - 1);
    pass = getpwuid(getuid());

    getcwd(pwd, sizeof(pwd) - 1);
    int len = strlen(pwd);
    char *ph = pwd + len - 1;
    while (*ph != '/' && len--)
        ph--;
    ph++;

    printf("\e[32;1m[%s @ %s %s]\n", pass->pw_name, name, ph);

    if(prompt_buffer[0] == '\0')
    {
        // 设置初始提示符
        char *arglist[] = {"myprompt", "$P$G"};
        inner(arglist);  // 调用inner函数处理命令
    }

    // 立即显示初始化后的提示符，而不是再次调用显示
    printf("\e[31;1m%s \e[0m", prompt_buffer);

    return;
}

int execute(char *arglist[]) // 执行外部指令
{
    int err;
    err = execvp(arglist[0], arglist);
    if (-1 == err)
    {
        printf("Execute Failed!\n");
        exit(-1);
    }
    return err;
}

char *make(char *buf) // 将字符传入参数表并分配空间
{
    char *arc;
    arc = malloc(strlen(buf) + 1);
    // 分配堆空间

    if (arc == NULL)
    {
        fprintf(stderr, "No Memory!\n");
        exit(1);
    }

    strcpy(arc, buf);
    return arc;
}

int mystrtok(char *str, char *delim) // 拆分传入参数
{
    char *token = NULL;
    // 原型函数strtok_r（）的Token指针

    char *save = NULL;
    // 保存原型函数strtok_r（）的指针

    argc = 0;
    // Parameter counter

    char chBuffer[MAXN];

    char *token1 = NULL;
    char *tmp = NULL;
    // 用于记录分段前的

    char *save1 = NULL;
    // 用于分割重定向符号和管道符号

    char *operator[] = {">>", "<<", ">", "<", "|"};
    int opline = sizeof(operator) / sizeof(operator[0]); // operator个数

    strncpy(chBuffer, str, sizeof(chBuffer) - 1);
    token = chBuffer;

    while (NULL != (token = strtok_r(token, delim, &save)))
    {
        for (int i = 0; i < opline; i++)
        {
            if (strlen(token) > 1 && find(token, operator[i]) != -1)  //若包含操作符
            {
                if (find(token, operator[i]) == 0) // 操作符在令牌开头
                {
                    arglist[argc++] = make(operator[i]);
                    token1 = strtok_r(token, operator[i], &save1);
                    arglist[argc++] = make(token1);
                }

                else if (find(token, operator[i]) == strlen(token) - 1) // 操作符在令牌结尾
                {
                    token1 = strtok_r(token, operator[i], &save1);
                    arglist[argc++] = make(token1);
                    arglist[argc++] = make(operator[i]);
                }

                else // 操作符在令牌中间
                {
                    token1 = strtok_r(token, operator[i], &save1);
                    arglist[argc++] = make(token1);

                    arglist[argc++] = make(operator[i]);
                    arglist[argc++] = make(save1);
                }

                token1 = NULL;
                save1 = NULL;

                strcmp(token, tmp);
                // 恢复token
            }
            // 操作符出现在字符中
        }

        arglist[argc++] = make(token); // 当前令牌存储到arglist
        token = NULL;
    }

    return argc;
}

int inner(char *arglist[]) // 执行内置指令
{
    if (strcmp(arglist[0], "exit\0") == 0) //exit
    {
        printf("GoodDay~\n");
        exit(0);
        return 1;
    }

    if (strcmp(arglist[0], "help\0") == 0) // help
    {
        printf("Bash, version 1.0-release (x86_64-pc-openEuler)\n");

        printf("These shell commands are defined internally.  Type 'help' to see this list.\n\n");
        printf("mycd\n");
        printf("mycp\n");
        printf("mydir\n");
        printf("myline\n");
        printf("mymd\n");
        printf("mymv\n");
        printf("myprompt\n");
        printf("myps\n");
        printf("myrd\n");
        printf("myren\n");
        printf("myrm\n");
        printf("mysort\n");
        printf("mytime\n");
        printf("mytree\n");
        printf("mytype\n");
        printf("myxcp\n");
        printf("exit\t\t: Exit the shell.\n");

        printf("\n");

        return 1;
    }

    else if (strcmp(arglist[0], "pwd\0") == 0) //pwd
    {
        char buf[LEN];
        getcwd(buf, sizeof(buf));
        // get current directory

        printf("%s\n\n", buf);
        return 1;
    }

    else if (strcmp(arglist[0], "mymd\0") == 0)
    {
        mymd(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "mycd\0") == 0) //cd
    {
        mycd(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "myren\0") == 0)
    {
        myren(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "mycp\0") == 0)
    {
        mycp(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "myps\0") == 0)
    {
        myps();
        return 1;
    }

    else if (strcmp(arglist[0], "mydir\0") == 0)
    {
        mydir(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "myxcp\0") == 0)
    {
        myxcp(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "myprompt\0") == 0)
    {
        myprompt(2, arglist, prompt_buffer, sizeof(prompt_buffer));

        if (prompt_buffer[0] != '\0') {

        } else {
            fprintf(stderr, "myprompt failed to generate a valid prompt\n");
        }
        return 1;
    }

    else if (strcmp(arglist[0], "mytime\0") == 0)
    {
        mytime(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "mytree\0") == 0)
    {
        mytree(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "myhis\0") == 0)
    {
        printf("-------------------------------------\n");
        printf("**  Print Input History until now: **\n");
        for (int i = 0; i < cmd_cnt; i++)
            printf("%s", history[i]);
        printf("-------------------------------------\n");
        return 1;
    }

    else if (strcmp(arglist[0], "mysort\0") == 0)
    {
        mysort(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "myrm\0") == 0)
    {
        myrm(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "myrd\0") == 0)
    {
        myrd(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "mymv\0") == 0)
    {
        mymv(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "mytype\0") == 0)
    {
        mytype(argc, arglist);
        return 1;
    }

    else if (strcmp(arglist[0], "myline\0") == 0)
    {
        myline(argc, arglist);
        return 1;
    }

    else
        return 0;
}

int callCommandWithRedi(int left, int right) // 要执行的指令间隔[左，右)不包含管道，可能包含重定向
{

    int inNum = 0, outNum = 0;
    char *inFile = NULL, *outFile = NULL;
    // 确定是否有重定向

    int endIdx = right;
    // 指令复位前的结束下标

    for (int i = left; i < right; ++i)
    {
        if (strcmp(arglist[i], COMMAND_IN) == 0 && strcmp(arglist[i], COMMAND_IN2) != 0)
        {

            inNum++;
            // 输入重定向

            if (i + 1 < right)
                inFile = arglist[i + 1];

            else
                return ERROR_MISS_PARAMETER;
            // 重定向符号后缺少文件名

            if (endIdx == right)
                endIdx = i;
        }
        else if (strcmp(arglist[i], COMMAND_OUT) == 0 && strcmp(arglist[i], COMMAND_OUT2) != 0)
        {
            // 输出重定向
            outNum++;
            if (i + 1 < right)
                outFile = arglist[i + 1];
            else
                return ERROR_MISS_PARAMETER;
            // 重定向符号后缺少文件名

            if (endIdx == right)
                endIdx = i;
        }
    }

    if (inNum == 1) // 处理重定向
    {
        FILE *fp = fopen(inFile, "r");
        if (fp == NULL)
            return ERROR_FILE_NOT_EXIST;
        // 输入重定向文件不存在

        fclose(fp);
    }

    if (inNum > 1)
        return ERROR_MANY_IN;

    else if (outNum > 1)
        return ERROR_MANY_OUT;

    int result = RESULT_NORMAL;
    pid_t pid = vfork();
    if (pid == -1)
    {
        result = ERROR_FORK;
    }
    else if (pid == 0)
    {

        if (inNum == 1)
            freopen(inFile, "r", stdin);
        if (outNum == 1)
            freopen(outFile, "w", stdout);
        // I / O重定向

        char *comm[MAXN];
        for (int i = left; i < endIdx; ++i)
            comm[i] = arglist[i];
        comm[endIdx] = NULL;
        execvp(comm[left], comm + left);
        // execute

        exit(errno);
        // 执行错误，返回errno
    }

    else
    {
        int status;
        waitpid(pid, &status, 0);
        int err = WEXITSTATUS(status);
        // 读取子进程的返回码

        if (err)
        {
            printf("Command Error!\n");
            printf("You may need \e[31;1m'help'\e[0m\n\n");
        }
    }

    return result;
}

int callCommandWithPipe(int left, int right) // 要执行的指令间隔[左，右]可能包含一个管道
{
    if (left >= right)
        return RESULT_NORMAL;
    // 确定是否有管道命令

    int pipeIdx = -1;
    for (int i = left; i < right; ++i)
    {
        if (strcmp(arglist[i], COMMAND_PIPE) == 0)
        {
            pipeIdx = i;
            break;
        }
    }

    if (pipeIdx == -1)
    {
        return callCommandWithRedi(left, right);
    }
    // 不包含管道命令

    else if (pipeIdx + 1 == right)
    {
        return ERROR_PIPE_MISS_PARAMETER;
    }
    // 管道命令‘|’没有后续命令，并且缺少参数

    int fds[2];
    if (pipe(fds) == -1)
    {
        return ERROR_PIPE;
    }

    int result = RESULT_NORMAL;
    pid_t pid = vfork();

    if (pid == -1)
    {
        result = ERROR_FORK;
    }
    else if (pid == 0)
    {
        //子进程执行单个命令
        close(fds[0]);
        dup2(fds[1], STDOUT_FILENO);
        //将标准输出重定向到FDS [1]

        close(fds[1]);

        result = callCommandWithRedi(left, pipeIdx);
        exit(result);
    }
    else
    {
        // 父进程递归地执行后续命令
        int status;
        waitpid(pid, &status, 0);
        int exitCode = WEXITSTATUS(status);

        if (exitCode != RESULT_NORMAL)
        {
            // 子进程的指令没有正常退出，打印错误信息

            char info[4096] = {0};
            char line[MAXN];
            close(fds[1]);
            dup2(fds[0], STDIN_FILENO);
            // 将标准输入重定向到fds[0]

            close(fds[0]);
            while (fgets(line, MAXN, stdin) != NULL)
            {
                // 读取子进程的错误信息
                strcat(info, line);
            }

            printf("%s", info);

            result = exitCode;
        }

        else if (pipeIdx + 1 < right)
        {
            close(fds[1]);
            dup2(fds[0], STDIN_FILENO);
            // 将标准输入重定向到fds[0]

            close(fds[0]);
            result = callCommandWithPipe(pipeIdx + 1, right);
            // 递归地执行后续指令
        }
    }

    return result;
}

int callCommand(int commandNum)
{
    // 用户用来执行用户输入的命令的函数
    pid_t pid = fork();
    if (pid == -1)
    {
        return ERROR_FORK;
    }
    else if (pid == 0)
    {

        /*获取标准输入输出的文件标识符*/
        int inFds = dup(STDIN_FILENO);
        int outFds = dup(STDOUT_FILENO);

        int result = callCommandWithPipe(0, commandNum);

        /*恢复标准输入和输出重定向*/
        dup2(inFds, STDIN_FILENO);
        dup2(outFds, STDOUT_FILENO);
        exit(result);
    }
    else //父进程，等待子进程结束
    {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

int main()
{
    char buf[MAXN];

    int pid;
    int result;

    cmd_cnt = 0;
    memset(history, 0x00, sizeof(history));
    // 初始化历史命令

    while (1)
    {
        Init();
        const char *prompt = getenv("SHELL_PROMPT");
        if (prompt && prompt[0] != '\0')
        {
            printf("\e[31;1m%s \e[0m", prompt);
        }
        else
        {
            printf("");
        }
        fflush(stdout);

        fgets(buf, BUFFSIZE, stdin);
        // 单行读取指令

        if (strcmp(buf, "\n") == 0)
        {
            printf("\n");
            continue;
        }

        strcpy(history[cmd_cnt], buf);
        cmd_cnt++;
        if (cmd_cnt >= MAXN)
        {
            cmd_cnt = 0;
            memset(history, 0x00, sizeof(history));
            printf("--------------------------------------------\n");
            printf(" Waring: Lack of space! Reset Input History!\n");
            printf("--------------------------------------------\n");
        }
        // reset history[][]

        memset(arglist, 0x00, sizeof(arglist));

        argc = mystrtok(buf, " \b\r\n\t");
        // 处理命令，分为多个参数

        int inner_flag;
        inner_flag = inner(arglist);
        // 内置指令判断

        if (inner_flag)
            continue;

        result = callCommand(argc);

        switch (result)
        {
        case ERROR_FORK:
            fprintf(stderr, "\e[31;1mError: Fork error.\n\e[0m");
            exit(ERROR_FORK);
        case ERROR_COMMAND:
            fprintf(stderr, "\e[31;1mError: Command not exist in shell.\n\e[0m");
            break;
        case ERROR_MANY_IN:
            fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", COMMAND_IN);
            break;
        case ERROR_MANY_OUT:
            fprintf(stderr, "\e[31;1mError: Too many redirection symbol \"%s\".\n\e[0m", COMMAND_OUT);
            break;
        case ERROR_FILE_NOT_EXIST:
            fprintf(stderr, "\e[31;1mError: Input redirection file not exist.\n\e[0m");
            break;
        case ERROR_MISS_PARAMETER:
            fprintf(stderr, "\e[31;1mError: Miss redirect file parameters.\n\e[0m");
            break;
        case ERROR_PIPE:
            fprintf(stderr, "\e[31;1mError: Open pipe error.\n\e[0m");
            break;
        case ERROR_PIPE_MISS_PARAMETER:
            fprintf(stderr, "\e[31;1mError: Miss pipe parameters.\n\e[0m");
            break;

            printf("\n");
        }
    }
    return 0;
}