#include "shell.h"

// 将权限模式转换成字符串
void mode2Char(int mode, char str[])
{
    strcpy(str, "----------");
    if (S_ISDIR(mode)) { str[0] = 'd'; }
    if (S_ISCHR(mode)) { str[0] = 'c'; }
    if (S_ISBLK(mode)) { str[0] = 'b'; }
    if ((mode & S_IRUSR)) { str[1] = 'r'; }
    if ((mode & S_IWUSR)) { str[2] = 'w'; }
    if ((mode & S_IXUSR)) { str[3] = 'x'; }
    if ((mode & S_IRGRP)) { str[4] = 'r'; }
    if ((mode & S_IWGRP)) { str[5] = 'w'; }
    if ((mode & S_IXGRP)) { str[6] = 'x'; }
    if ((mode & S_IROTH)) { str[7] = 'r'; }
    if ((mode & S_IWOTH)) { str[8] = 'w'; }
    if ((mode & S_IXOTH)) { str[9] = 'x'; }
}

// 将用户ID转换成用户名
char *uidToName(uid_t uid)
{
    struct passwd *pw_ptr;
    static char tmp_str[10];
    if ((pw_ptr = getpwuid(uid)) == NULL)
    {
        sprintf(tmp_str, "%d", uid);
        return tmp_str;
    }
    else
    {
        return pw_ptr->pw_name;
    }
}

// 将组ID转换成组名
char *gid2Name(gid_t gid)
{
    struct group *grp_ptr;
    static char tmp_str[10];
    if ((grp_ptr = getgrgid(gid)) == NULL)
    {
        sprintf(tmp_str, "%d", gid);
        return tmp_str;
    }
    else
    {
        return grp_ptr->gr_name;
    }
}

// 以人类可读方式显示文件大小
void printFileSize(long size)
{
    const char *units[] = {"B", "KB", "MB", "GB"};
    int unit_index = 0;
    float file_size = (float)size;
    while (file_size >= 1024 && unit_index < 3)
    {
        file_size /= 1024;
        unit_index++;
    }
    printf("%.2f%s ", file_size, units[unit_index]);
}

// 显示文件的十六进制表示
void printHex(char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("fopen");
        return;
    }
    int ch;
    while ((ch = fgetc(file)) != EOF)
    {
        printf("%02x ", ch);
    }
    fclose(file);
}

// 显示文件信息
void printFileInfo(char *filepath, char *filename, struct stat *info_p, int arg_mode)
{
    if (((arg_mode >> 0) & 1) && filename[0] == '.') // 处理隐藏文件
    {
        printf("%s\n",filename);
        return;
    }

    if ((arg_mode >> 5) & 1) // 处理大小写转换
    {
        for (int i = 0; filename[i]; i++)
        {
            filename[i] = tolower(filename[i]);
        }
        printf("%s\n",filename);
    }

    if ((arg_mode >> 4) & 1) // 处理完整路径和时间
    {
        printf("%s\n", filepath);
        printf("%s\n", ctime(&info_p->st_mtime));
        return;
    }

    if ((arg_mode >> 6) & 1) // 处理完整路径
    {
        printf("%s\n", filepath);
        return;
    }

    if ((arg_mode >> 16) & 1) // 处理文件类型标识符
    {
        printf("%s", filename);
        if (S_ISDIR(info_p->st_mode)) printf("/");
        else if (info_p->st_mode & S_IXUSR) printf("*");
        else printf("\n");
        return;
    }


    if ((arg_mode >> 1) & 1) // 处理详细列表
    {
        char modestr[11];
        mode2Char(info_p->st_mode, modestr);
        printf("%s ", modestr);

        printf("%4d ", (int)info_p->st_nlink);
        printf("%-8s ", uidToName(info_p->st_uid));
        printf("%-8s ", gid2Name(info_p->st_gid));

        if ((arg_mode >> 17) & 1) // 处理人类可读的文件大小
        {
            printFileSize(info_p->st_size);
        }
        else
        {
            printf("%8ld ", (long)info_p->st_size);
        }

        if ((arg_mode >> 12) & 1) // 处理时间戳
        {
            printf("%.12s ", 4 + ctime(&info_p->st_mtime));
        }
        else
        {
            printf("%.12s ", 4 + ctime(&info_p->st_ctime));
        }

        printf("%s\n", filename);
    }

    else if ((arg_mode >> 2) & 1) // 处理简短列表
    {
        printf("%s\n", filename);
    }

    else if ((arg_mode >> 18) & 1)
    {
        printf("%-15s",filename);
    }

    if ((arg_mode >> 14) & 1) // 处理十六进制表示
    {
        printHex(filename);
    }

    if ((arg_mode >> 3) & 1) // 处理显示文件大小
    {
        printf("%8ld %s\n", (long)info_p->st_size, filename);
        printf("\n");
    }

    if ((arg_mode >> 13) & 1) // 宽列表格式显示
    {
        printf("%-20s", filename);
    }

    if ((arg_mode >> 15) & 1) // 处理四字符宽度
    {
        printf("%-4s", filename);
    }

    //
    if ((arg_mode >> 17) & 1) // 处理人类可读的文件大小
    {
        printf("%s\n",filename);
        printFileSize(info_p->st_size);
    }

    if ((arg_mode >> 12) & 1) // 处理时间戳
    {
        printf("%s\t\t\t",filepath);
        printf("%.12s \n", 4 + ctime(&info_p->st_mtime));
    }
    if ((arg_mode >> 9) & 1) // 处理所有者信息
    {

        printf("%s\t\t\t",filename);
        printf("%-8s\n", uidToName(info_p->st_uid));
    }
    if ((arg_mode >> 10) & 1) // 处理递归列表
    {
        printf("\t\t\t\t%s\n",filename);
    }
    if ((arg_mode >> 11) & 1) // 处理非递归列表
    {

    }
    //
}

// 显示目录内容（递归）
void listDir(const char *dirname, int arg_mode, int depth)
{
    DIR *dir_ptr;
    struct dirent *direntp;

    if ((dir_ptr = opendir(dirname)) == NULL)
    {
        fprintf(stderr, "ls: cannot open %s\n", dirname);
    }
    else
    {
        while ((direntp = readdir(dir_ptr)) != NULL)
        {
            struct stat info;
            char filepath[PATH_MAX];
            snprintf(filepath, sizeof(filepath), "%s/%s", dirname, direntp->d_name);

            if (stat(filepath, &info) == -1)
            {
                perror(filepath);
            }
            else
            {
                if (!((arg_mode >> 0) & 1) && direntp->d_name[0] == '.') // 处理隐藏文件
                {
                    continue;
                }

                // 处理属性过滤
                if ((arg_mode >> 3) & 1) // 仅目录
                {
                    if (!S_ISDIR(info.st_mode))
                        continue;
                }
                if ((arg_mode >> 3) & 2) // 只读文件
                {
                    if (info.st_mode & S_IWUSR)
                        continue;
                }
                if ((arg_mode >> 3) & 4) // 隐藏文件
                {
                    if (direntp->d_name[0] != '.')
                        continue;
                }
                if ((arg_mode >> 3) & 8) // 归档文件
                {
                    // 实现归档文件检查（如有需要）
                }
                if ((arg_mode >> 3) & 16) // 系统文件
                {
                    // 实现系统文件检查（如有需要）
                }

                printFileInfo(filepath, direntp->d_name, &info, arg_mode);

                if ((arg_mode >> 10) & 1) // 处理递归列表
                {
                    if (S_ISDIR(info.st_mode) && strcmp(direntp->d_name, ".") != 0 && strcmp(direntp->d_name, "..") != 0)
                    {
                        printFileInfo(filepath, direntp->d_name, &info, arg_mode);
                        for (int i = 0; i < depth; i++) printf("    ");
                        printf("%s：\n", filepath);
                        listDir(filepath, arg_mode, depth + 1);
                    }
                }
                if((arg_mode >> 11) & 1)
                {
                    printFileInfo(filepath, direntp->d_name, &info, arg_mode);
                    for (int i = 0; i < depth; i++) printf("    ");
                    printf("%s：\n", filepath);
                }
            }
        }
        closedir(dir_ptr);
    }
}

// 按名称排序
int compareByName(const void *a, const void *b)
{
    const struct dirent *entryA = *(const struct dirent **)a;
    const struct dirent *entryB = *(const struct dirent **)b;
    return strcmp(entryA->d_name, entryB->d_name);
}

// 按大小排序
int compareBySize(const void *a, const void *b)
{
    struct stat stat1, stat2;
    const struct dirent *entryA = *(const struct dirent **)a;
    const struct dirent *entryB = *(const struct dirent **)b;
    stat(entryA->d_name, &stat1);
    stat(entryB->d_name, &stat2);
    return (int)(stat1.st_size - stat2.st_size);
}

// 按时间排序
int compareByTime(const void *a, const void *b)
{
    struct stat stat1, stat2;
    const struct dirent *entryA = *(const struct dirent **)a;
    const struct dirent *entryB = *(const struct dirent **)b;
    stat(entryA->d_name, &stat1);
    stat(entryB->d_name, &stat2);
    return (int)(stat2.st_mtime - stat1.st_mtime);
}

// 按扩展名排序
int compareByExtension(const void *a, const void *b)
{
    const struct dirent *entryA = *(const struct dirent **)a;
    const struct dirent *entryB = *(const struct dirent **)b;

    // 提取扩展名
    const char *extA = strrchr(entryA->d_name, '.');
    const char *extB = strrchr(entryB->d_name, '.');

    // 如果没有扩展名，将其视为“空”扩展名
    extA = extA ? extA : "";
    extB = extB ? extB : "";

    return strcmp(extA, extB);
}

// 按组排序
int compareByGroup(const void *a, const void *b)
{
    struct stat stat1, stat2;
    const struct dirent *entryA = *(const struct dirent **)a;
    const struct dirent *entryB = *(const struct dirent **)b;

    stat(entryA->d_name, &stat1);
    stat(entryB->d_name, &stat2);

    return (int)(stat1.st_gid - stat2.st_gid);
}




// 排序文件列表
// 排序文件列表
void sortFiles(struct dirent **namelist, int n, char sort_order)
{
    int (*compare)(const void *, const void *);

    switch (sort_order)
    {
        case 'n':
            compare = compareByName; // 按名称排序
            break;
        case 'e':
            compare = compareByExtension; // 按扩展名排序（可以扩展实现）
            break;
        case 'd':
            compare = compareByTime; // 按修改日期排序
            break;
        case 's':
            compare = compareBySize; // 按大小排序
            break;
        case 'g':
            compare = compareByGroup; // 按组排序（可以扩展实现）
            break;
        default:
            return;
    }

    if (compare != NULL)
    {
        printf("Sorting files...\n");
        for (int i = 0; i < n; i++) {
            // printf("File before sorting: %s\n", namelist[i]->d_name);
        }

        qsort(namelist, n, sizeof(struct dirent *), (int (*)(const void *, const void *))compare);

        for (int i = 0; i < n; i++) {
            // printf("File after sorting: %s\n", namelist[i]->d_name);
            printf("%s\n", namelist[i]->d_name);
        }
    }
}

// 主函数myls
void mydir(int num, char *arglist[]) {
    int arg_mode = 0;
    char sort_order = 0;
    char *sort_order_options = "neds";
    int reverse_sort = 0;

    // 处理命令行参数
    if (num != 1) {
        int arg_len = strlen(arglist[1]);
        for (int i = 1; i < arg_len; i++) {
            switch (arglist[1][i]) {
                case 'a':
                    arg_mode |= (1 << 0);
                    break; // 实现 -a 显示隐藏文件
                case 'l':
                    arg_mode |= (1 << 1);
                    break; // 实现详细列表显示
                case 'b':
                    arg_mode |= (1 << 2);
                    break; // 仅显示文件名和扩展名
                case 'c':
                    arg_mode |= (1 << 3);
                    break; // 显示文件大小
                case 'd':
                    arg_mode |= (1 << 4);
                    break; // 显示日期和最后修改时间
                case 'L':
                    arg_mode |= (1 << 5);
                    break; // 以小写字母显示文件和目录名
                case 'n':
                    arg_mode |= (1 << 6);
                    break; // 显示寄文件完整路径
                case 'o':
                    char *opr;
                    size_t opr_len;
                    if (i + 1 < arg_len) {
                        opr = arglist[i];
                        printf("%s\n", opr);
                        opr_len = strlen(opr);
                        printf("Order: %c\n", opr[opr_len - 1]);
                        DIR *dir_ptr;
                        struct dirent **namelist;
                        if ((dir_ptr = opendir(".")) == NULL) {
                            fprintf(stderr, "ls: cannot open %s\n", ".");
                        } else {
                            int n = scandir(".", &namelist, NULL, alphasort);
                            if (n < 0) {
                                perror("scandir");
                            } else {
                                char oprIn = opr[opr_len - 1]; // 使用命令行参数最后一个字符作为排序顺序
                                if (oprIn != 0) {
                                    sortFiles(namelist, n, oprIn);
                                }

                                if ((arg_mode >> 10) & 1) { // 递归显示目录内容
                                    for (int i = 0; i < n; i++) {
                                        char filepath[PATH_MAX];
                                        realpath(namelist[i]->d_name, filepath);
                                        if (namelist[i]->d_type == DT_DIR) {
                                            printf("%s:\n", filepath);
                                            listDir(filepath, arg_mode, 0);
                                        }
                                        free(namelist[i]);
                                    }
                                    free(namelist);
                                    closedir(dir_ptr);
                                    return;
                                }

                                for (int i = 0; i < n; i++) {
                                    struct stat info;
                                    if (stat(namelist[i]->d_name, &info) == -1) {
                                        perror(namelist[i]->d_name);
                                    } else {
                                        char filepath[PATH_MAX];
                                        realpath(namelist[i]->d_name, filepath);
                                        printFileInfo(filepath, namelist[i]->d_name, &info, arg_mode);

                                        if ((arg_mode >> 8) & 1) { // 分屏显示输出
                                            static int counter = 0;
                                            counter++;
                                            if (counter % 10 == 0) {
                                                printf("Press any key to continue...");
                                                getchar();
                                            }
                                        }
                                    }
                                    free(namelist[i]);
                                }
                                free(namelist);
                            }
                            closedir(dir_ptr);
                        }
                        arg_mode |= (1 << 7);
                        goto flag1;
                        break;
                    }
                case 'p':
                    arg_mode |= (1 << 8);
                    break; // 分屏显示输出
                case 'q':
                    arg_mode |= (1 << 9);
                    break; // 显示文件所有者信息
                case 'r':
                    char *current_dir = getcwd(NULL, 0);
                    mytree(2, (char *[]){"mytree", current_dir});
                    free(current_dir);
                    // arg_mode |=(1 << 10);
                    break; // 递归显示目录下所有文件和子目录
                case 's':
                    char *path;
                    if (i + 1 < arg_len) {
                        path = arglist[1] + i + 1;
                        while (arglist[1][i + 1] && arglist[1][i + 1] != ' ') {
                            i++;
                        }
                    } else {
                        fprintf(stderr, "Option 's' requires a path argument.\n");
                        mydir(1, (char *[]){"mydir", current_dir});
                        break;
                        // 如果没有输入参数，在后续处理会导致内存越界引发程序终止，因此手动回到main函数
                        // 回到main函数会导致myhis重置，因此break
                    }
                    printf("%s\n", path);
                    arg_mode |= (1 << 11);
                    if (path) {
                        listDir(path, arg_mode, 10); // 传递当前目录和 global_path 到 listDir
                    } else {
                        fprintf(stderr, "No path provided for option 's'.\n");
                    }

                    break; // 显示指定路径下的所有文件和子目录
                case 'T':
                    arg_mode |= (1 << 12);
                    break; // 显示文件的最后修改时间、创建时间或访问时间
                case 'w':
                    arg_mode |= (1 << 13);
                    break; // 使用宽列表格式显示
                case 'x':
                    arg_mode |= (1 << 14);
                    break; // 显示十六进制表示
                case '4':
                    arg_mode |= (1 << 15);
                    break; // 四字符宽度显示
                case 'f':
                    arg_mode |= (1 << 16);
                    break; // 添加文件类型标识符
                case 'h':
                    arg_mode |= (1 << 17);
                    break; // 以人类可读的方式显示文件大小
                default:
                    //arg_mode |= (1 << 18);
                    break; // 仅显示文件名和扩展名
            }
        }
    }
    else
    {
        arg_mode |= (1 << 18);
    }
    DIR *dir_ptr;
    struct dirent **namelist;
    if ((dir_ptr = opendir(".")) == NULL)
    {
        fprintf(stderr, "dir: cannot open %s \n", ".");
    }
    else
    {
        int n = scandir(".", &namelist, NULL, alphasort);
        if (n < 0)
        {
            perror("scandir");
        }
        else
        {
            if (sort_order != 0)
            {
                sortFiles(namelist, n, sort_order);
            }

            if (reverse_sort)
            {
                for (int i = 0; i < n / 2; i++)
                {
                    struct dirent *tmp = namelist[i];
                    namelist[i] = namelist[n - i - 1];
                    namelist[n - i - 1] = tmp;
                }
            }

            for (int i = 0; i < n; i++)
            {
                struct stat info;
                if (stat(namelist[i]->d_name, &info) == -1)
                {
                    perror(namelist[i]->d_name);
                }
                else
                {
                    char filepath[PATH_MAX];
                    realpath(namelist[i]->d_name, filepath);
                    printFileInfo(filepath, namelist[i]->d_name, &info, arg_mode);

                    if ((arg_mode >> 8) & 1) // 分屏显示输出
                    {
                        static int counter = 0;
                        counter++;
                        if (counter % 10 == 0)
                        {
                            printf("Press any key to continue...");
                            getchar();
                        }
                    }
                }
                free(namelist[i]);
            }
            free(namelist);
        }
        closedir(dir_ptr);
    }
    flag1:
    printf("\n");
}
