#include "shell.h"

#define REN_SUCCESS 1
#define REN_FAILURE 2

void print_usage_ren() {
    puts("Usage: myren [path] <old_name> <new_name>");
}

int myren_main(const char *path, const char *old_name, const char *new_name) {
    char old_path[PATH_MAX];
    char new_path[PATH_MAX];

    snprintf(old_path, sizeof(old_path), "%s/%s", path, old_name);
    snprintf(new_path, sizeof(new_path), "%s/%s", path, new_name);
    // 拼出正确路径

    // 检查旧路径是否存在
    struct stat st;
    if (lstat(old_path, &st)) {
        printf("Source file or directory does not exist: %s\n", old_path);
        return REN_FAILURE;
    }

    // 检查新路径是否已存在
    if (!lstat(new_path, &st)) {
        printf("Destination file or directory already exists: %s\n", new_path);
        return REN_FAILURE;
    }

    // 重命名文件或目录
    if (rename(old_path, new_path) == 0) {
        printf("Successfully renamed %s to %s\n", old_name, new_name);
        return REN_SUCCESS;
    } else {
        perror("rename");
        return REN_FAILURE;
    }
}

int myren(int argc, char *argv[]) {
    const char *path = ".";
    const char *old_name;
    const char *new_name;

    if (argc == 3) {
        old_name = argv[1];
        new_name = argv[2];
    } else if (argc == 4) {
        path = argv[1];
        old_name = argv[2];
        new_name = argv[3];
    } else {
        print_usage_ren();
        return REN_FAILURE;
    }

    return myren_main(path, old_name, new_name);
}
