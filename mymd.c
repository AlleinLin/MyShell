#include "shell.h"

#define MD_SUCCESS 1
#define MD_FAILURE 2

void print_usage_mkdir() {
    puts("Usage: mymd <directory> [directory ...]");
}

// 创建目录
int mymd_main(char *target) {
    if (mkdir(target, 0777) == 0) {
        printf("Successfully created directory %s\n", target);
        return MD_SUCCESS;
    } else {
        printf("Failed to create directory %s\n", target);
        return MD_FAILURE;
    }
}

int mymd(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage_mkdir();
        return MD_FAILURE;
    }

    int result = MD_SUCCESS;
    for (int i = 1; i < argc; i++) {
        char *target = argv[i];
        result = mymd_main(target);
        if (result == MD_FAILURE) {
            break;
        }
    }

    return result;
}
