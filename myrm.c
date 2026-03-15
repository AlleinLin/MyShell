#include "shell.h"

#define RM_SUCCESS 1
#define RM_FAILURE 2

int myrm_main(char *target, int mode, int confirm, int force, int quiet);

void print_usage_del() {
    puts("Usage: myrm [options] <path>");
    puts("Options:");
    puts("  -p  : 在每个文件删除时确认");
    puts("  -f  : 强制删除只读文件");
    puts("  -s  : 删除指定目录及其所有子目录中的指定文件");
    puts("  -q  : 安静模式，删除文件时不提示确认");
}

// 循环且强制删除文件和目录
int myrm_main(char *target, int mode, int confirm, int force, int quiet) {
    DIR *dir;
    dir = opendir(target);

    // file or nothing
    if (dir == NULL) {
        int flag = 0;

        // 处理只读文件
        if (force) {
            chmod(target, S_IWUSR);
        }

        // 删除前确认
        if (confirm) {
            printf("Are you sure you want to delete %s? (y/n): ", target);
            int response = getchar();
            if (response != 'y' && response != 'Y') {
                return RM_FAILURE;
            }
        }

        // 删除文件
        flag = unlink(target);

        if (flag == 0 && !quiet) {
            printf("Successfully removed %s\n", target);
            return RM_SUCCESS;
        } else {
            return RM_FAILURE;
        }
    }

    // 获得当前工作路径
    char pwd[NAME_MAX];
    memset(pwd, 0, sizeof(pwd));
    if (getcwd(pwd, NAME_MAX) == NULL) {
        printf("Error in getting pwd\n");
        return RM_FAILURE;
    }

    // 处理目录
    struct dirent *dp;
    while (1) {
        dp = readdir(dir);

        // 到达末尾
        if (dp == NULL) {
            break;
        }

        // 获得名称
        char tar_sub[NAME_MAX];
        memset(tar_sub, 0, sizeof(tar_sub));
        sprintf(tar_sub, "%s", dp->d_name);

        // 处理 "." and ".."
        if (strcmp(tar_sub, ".") == 0 || strcmp(tar_sub, "..") == 0) {
            continue;
        }

        // 改变工作目录
        chdir(target);

        // 循环删除文件和目录
        if (myrm_main(tar_sub, mode, confirm, force, quiet) == RM_FAILURE) {
            return RM_FAILURE;
        }
    }

    // 必须关闭它然后再将其移除
    closedir(dir);

    // 切换回来
    chdir(pwd);

    // 删除当前目录
    rmdir(target);

    if (!quiet) {
        printf("Successfully removed %s\n", target);
    }

    return RM_SUCCESS;
}

int myrm(int argc, char *argv[]) {
    int opt;
    int confirm = 0, force = 0, mode = 1, quiet = 0;

    while ((opt = getopt(argc, argv, "pfsq")) != -1) {
        switch (opt) {
            case 'p':
                confirm = 1;
                break;
            case 'f':
                force = 1;
                break;
            case 's':
                mode = 1;
                break;
            case 'q':
                quiet = 1;
                break;
            default:
                print_usage_del();
                return RM_FAILURE;
        }
    }

    // 剩余点应该是目标路径
    if (optind >= argc) {
        print_usage_del();
        return RM_FAILURE;
    }

    char *target = argv[optind];
    int result = myrm_main(target, mode, confirm, force, quiet);

    return result;
}

int mvrm(int argc, char *argv[])
{
    // 处理参数
    if (argc < 2)

    {
        puts("Parameter error!");
        puts("Usage1: myrm <directory>");
        puts("Usage1: myrm <file>");
        return RM_FAILURE;
    }

    if (argc > 2)

    {
        puts("Parameter error!");
        puts("Usage1: myrm <directory>");
        puts("Usage1: myrm <file>");
        return RM_FAILURE;
    }

    // 循环且强制删除文件和目录
    int result = myrm_main(argv[1], 0, 0, 0, 1);

    return result;
}