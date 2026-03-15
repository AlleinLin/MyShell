# include "shell.h"

// 获取一个文件的行数
int myline4file(char *path)
{
    // open file
    FILE *fp;
    fp = fopen(path, "r");

    if (fp == NULL)

    {
        puts("Can NOT open file!");
        return -1;
    }

    char ch;
    int row = 1, cnt = 0;

    // 开始统计行数
    while (feof(fp) == 0)
    {
        ch = fgetc(fp);
        cnt++;

        if (ch == '\n')

        {
            row++;
        }
    }

    fclose(fp);

    printf("%s      \t%d\n", path, row);

    return row;
}

// 获取一个目录或文件的总行数
int Myline(char *path)
{
    int total_lines = 0;

    // 获取信息
    struct stat st;
    stat(path, &st);

    // 路径指向一个文件
    if (S_ISREG(st.st_mode))

    {
        total_lines = myline4file(path);
        return total_lines;
    }

    // 路径指向一个目录
    DIR *dir;
    dir = opendir(path);

    // 获取当前工作路径
    char pwd[NAME_MAX];
    memset(pwd, 0, sizeof(pwd));
    if (getcwd(pwd, NAME_MAX) == NULL)
    {
        puts("Error in geting pwd!");
        return -1;
    }

    // 处理目录
    struct dirent *dp;
    while (1)
    {
        dp = readdir(dir);

        // 到达末尾
        if (dp == NULL)

        {
            break;
        }

        // 获取名称
        char child_name[NAME_MAX];
        memset(child_name, 0, sizeof(child_name));
        sprintf(child_name, "%s", dp->d_name);

        // deal with "."
        if (strcmp(child_name, ".") == 0)

        {
            continue;
        }

        // deal with ".."
        if (strcmp(child_name, "..") == 0)

        {
            continue;
        }

        unsigned char child_type = dp->d_type;

        // 更改工作目录
        chdir(path);

        // 递归处理文件和目录
        if (child_type == 4)

        {
            int line_sub;
            line_sub = Myline(child_name);
            if (line_sub == -1)
            {
                return -1;
            }
            else
            {
                total_lines += line_sub;
            }
        }

        else if (child_type == 8)

        {
            int line_sub;
            line_sub = myline4file(child_name);
            if (line_sub == -1)
            {
                return -1;
            }
            else
            {
                total_lines += line_sub;
            }
        }
    }

    // 在删除之前必须关闭它
    closedir(dir);

    // 切换回原目录
    chdir(pwd);

    return total_lines;
}

int myline(int argc, char *argv[])
{
    // 处理参数
    if (argc < 2)
    {
        puts("Parameter error!");
        puts("Usage1: myline <dir>");
        puts("Usage2: myline <file>");
        return -1;
    }

    if (argc > 2)
    {
        puts("Parameter error!");
        puts("Usage1: myline <dir>");
        puts("Usage2: myline <file>");
        return -1;
    }

    // 统计总行数
    int total_lines = Myline(argv[1]);

    if (total_lines != -1)
    {
        printf("\ntotal lines: \t%d\n\n", total_lines);
    }

    return total_lines;
}