#include "shell.h"

#define RM_SUCCESS 1
#define RM_FAILURE 2

void print_usage_rd() {
    puts("Usage: myrd [options] <path>");
    puts("Options:");
    puts("  -s  : 删除目录及其所有子目录和文件");
    puts("  -q  : 删除目录时不进行确认提示");
}

// 循环删除目录
int myrd_main(char *target, int recursive, int quiet) {
    struct stat statbuf;
    stat(target, &statbuf);

    // 检查目标是否是目录
    if (S_ISDIR(statbuf.st_mode)) {
        DIR *dir = opendir(target);
        struct dirent *dp;

        if (dir == NULL) {
            printf("Failed to open directory: %s\n", target);
            return RM_FAILURE;
        }
        printf("Opened directory: %s\n", target);

        // 如果设置了-s选项，则以递归方式删除内容
        if (recursive) {
            while ((dp = readdir(dir)) != NULL) {
                // 忽略 "." and ".."
                if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                    continue;
                }

                // 构造路径
                char path[NAME_MAX];
                snprintf(path, sizeof(path), "%s/%s", target, dp->d_name);

                // 递归
                if (myrd_main(path, recursive, quiet) == RM_FAILURE) {
                    closedir(dir);
                    return RM_FAILURE;
                }
            }
        }

        closedir(dir);

        // 删除目标目录本身
        printf("Attempting to remove directory: %s\n", target);
        if (rmdir(target) == 0) {
            if (!quiet) {
                printf("Successfully removed directory %s\n", target);
            }
            return RM_SUCCESS;
        } else {
            printf("Failed to remove directory %s\n", target);
            return RM_FAILURE;
        }
    } else {
        // 目标是文件
        printf("Attempting to remove file: %s\n", target);
        if (unlink(target) == 0) {
            if (!quiet) {
                printf("Successfully removed file %s\n", target);
            }
            return RM_SUCCESS;
        } else {
            printf("Failed to remove file %s\n", target);
            return RM_FAILURE;
        }
    }
}


int myrd(int argc, char *argv[]) {
    int opt;
    int recursive = 0, quiet = 0;

    while ((opt = getopt(argc, argv, "sq")) != -1) {
        switch (opt) {
            case 's':
                recursive = 1;
                break;
            case 'q':
                quiet = 1;
                break;
            default:
                print_usage_rd();
                return RM_FAILURE;
        }
    }

    // 剩下的参数应该是目标路径
    if (optind >= argc) {
        print_usage_rd();
        return RM_FAILURE;
    }

    char *target = argv[optind];
    int result = myrd_main(target, recursive, quiet);

    return result;
}
