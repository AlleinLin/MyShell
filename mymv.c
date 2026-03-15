#include "shell.h"

#define tpDIRECTORY 4
#define tpFILE 8
#define tpLINK 10

int mvrm(int argc, char *argv[]);  // 申明mvrm函数

int mymv_main(char *src, char *tar, int auto_confirm) {
    // 获取源目录信息
    struct stat st;
    if (lstat(src, &st)) {
        printf("lstat error\n");
        return 2;
    }

    // src若为文件
    if (S_ISREG(st.st_mode)) {
        int fd_src = open(src, O_RDONLY);
        if (fd_src < 0) {
            printf("open error\n");
            return 0;
        }

        // 检查目标文件是否存在
        if (access(tar, F_OK) == 0 && !auto_confirm) {
            printf("Target file %s exists. Overwrite? (y/n): ", tar);
            int response = getchar();
            if (response != 'y' && response != 'Y') {
                close(fd_src);
                return 2;
            }
        }

        // 创建新文件
        int fd_tar = open(tar, O_RDWR | O_CREAT | O_TRUNC, st.st_mode);
        if (fd_tar < 0) {
            printf("open error\n");
            close(fd_src);
            return 0;
        }

        int len;
        char buffer[BUFSIZ];
        while ((len = read(fd_src, buffer, BUFSIZ)) > 0) {
            write(fd_tar, buffer, len);
        }

        close(fd_src);
        close(fd_tar);

        // 改变时间
        struct timespec ts[2];
        ts[0] = st.st_atim;
        ts[1] = st.st_mtim;
        if (utimensat(AT_FDCWD, tar, ts, 0)) {
            printf("utimensat error\n");
            return 2;
        }

        char *argv[] = {"mvrm", src, NULL};
        return mvrm(2, argv);
    }

    // 若src是一个目录
    if (mkdir(tar, st.st_mode)) {
        printf("mkdir error\n");
        return 2;
    }

    DIR *dir = opendir(src);
    if (!dir) {
        printf("opendir error\n");
        return 2;
    }

    struct dirent *de;
    while ((de = readdir(dir))) {
        // 获得名字
        char name[NAME_MAX];
        strcpy(name, de->d_name);

        // 处理 "." and ".."
        if (!strcmp(name, ".") || !strcmp(name, "..")) {
            continue;
        }

        // 获得完整路径
        char src_sub[NAME_MAX], tar_sub[NAME_MAX];
        sprintf(src_sub, "%s/%s", src, name);
        sprintf(tar_sub, "%s/%s", tar, name);

        // 创建
        if (mymv_main(src_sub, tar_sub, auto_confirm) != 1) {
            printf("mymv_main error\n");
            closedir(dir);
            return 2;
        }
    }

    closedir(dir);

    // 修改时间
    struct timespec ts[2];
    ts[0] = st.st_atim;
    ts[1] = st.st_mtim;
    if (utimensat(AT_FDCWD, tar, ts, 0)) {
        printf("utimensat error\n");
        return 2;
    }

    char *argv[] = {"mvrm", src, NULL};
    return mvrm(2, argv);
}

int mymv(int argc, char *argv[]) {
    int opt;
    int auto_confirm = 0;

    while ((opt = getopt(argc, argv, "y")) != -1) {
        switch (opt) {
            case 'y':
                auto_confirm = 1;
                break;
            default:
                puts("Parameter error!");
                puts("Usage: mymv [-y] <source> <destination>");
                return 2;
        }
    }

    // 剩下的参数应该是源和目标
    if (optind + 2 != argc) {
        puts("Parameter error!");
        puts("Usage: mymv [-y] <source> <destination>");
        return 2;
    }

    char *src = argv[optind];
    char *tar = argv[optind + 1];

    // 提示用户是否移动文件
    if (!auto_confirm) {
        printf("Move file from %s to %s? (y/n): ", src, tar);
        int response = getchar();
        if (response != 'y' && response != 'Y') {
            return 2;
        }
    }

    // 开始移动
    int flag = mymv_main(src, tar, auto_confirm);

    if (flag == 1) {
        puts("Successfully moved.");
    } else {
        return 2;
    }
    return 0;
}
