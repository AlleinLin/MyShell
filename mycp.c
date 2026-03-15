#include "shell.h"

#define CP_SUCCESS 1
#define CP_FAILURE 2

void print_usage_cp() {
    puts("Usage: mycp [options] <source> <destination>");
    puts("Options:");
    puts("  -y      : 默认覆盖文件");
    puts("  --y     : 默认取消覆盖文件");
    puts("  -v      : 验证每个新文件复制是否正确无误");
}

int copy_file(const char *src, const char *dest, int overwrite, int verify) {
    struct stat st;
    if (lstat(src, &st) < 0) {
        perror("lstat");
        return CP_FAILURE;
    }

    if (access(dest, F_OK) == 0 && !overwrite) {
        printf("Target file %s exists. Overwrite? (y/n): ", dest);
        int response = getchar();
        if (response != 'y' && response != 'Y') {
            return CP_FAILURE;
        }
    }

    int fd_src = open(src, O_RDONLY);
    if (fd_src < 0) {
        perror("open src");
        return CP_FAILURE;
    }

    int fd_dest = open(dest, O_RDWR | O_CREAT | O_TRUNC, st.st_mode);
    if (fd_dest < 0) {
        perror("open dest");
        close(fd_src);
        return CP_FAILURE;
    }

    char buffer[BUFSIZ];
    ssize_t len;
    while ((len = read(fd_src, buffer, BUFSIZ)) > 0) {
        if (write(fd_dest, buffer, len) != len) {
            perror("write");
            close(fd_src);
            close(fd_dest);
            return CP_FAILURE;
        }
    }

    if (verify) {
        struct stat st_src, st_dest;
        fstat(fd_src, &st_src);
        fstat(fd_dest, &st_dest);
        if (st_src.st_size != st_dest.st_size) {
            printf("Verification failed: %s and %s sizes do not match.\n", src, dest);
            close(fd_src);
            close(fd_dest);
            return CP_FAILURE;
        }
    }

    close(fd_src);
    close(fd_dest);
    return CP_SUCCESS;
}

int concatenate_files(const char *files, const char *dest, int overwrite) {
    int fd_dest = open(dest, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd_dest < 0) {
        perror("open dest");
        return CP_FAILURE;
    }

    char *files_copy = strdup(files);
    if (!files_copy) {
        perror("strdup");
        close(fd_dest);
        return CP_FAILURE;
    }

    char *file = strtok(files_copy, "+");
    while (file != NULL) {
        int fd_src = open(file, O_RDONLY);
        if (fd_src < 0) {
            perror("open src");
            close(fd_dest);
            free(files_copy);
            return CP_FAILURE;
        }

        char buffer[BUFSIZ];
        ssize_t len;
        while ((len = read(fd_src, buffer, BUFSIZ)) > 0) {
            if (write(fd_dest, buffer, len) != len) {
                perror("write");
                close(fd_src);
                close(fd_dest);
                free(files_copy);
                return CP_FAILURE;
            }
        }

        close(fd_src);
        file = strtok(NULL, "+");
    }

    close(fd_dest);
    free(files_copy);
    return CP_SUCCESS;
}

int mycp_main(const char *src, const char *dest, int overwrite, int verify) {
    if (strchr(src, '+') != NULL) {
        return concatenate_files(src, dest, overwrite);
    } else {
        return copy_file(src, dest, overwrite, verify);
    }
}

int check(int argc, char *argv[], struct stat *statbuf) {
    if (argc < 3) {
        print_usage_cp();
        return CP_FAILURE;
    }

    // 只检查源文件存在性
    if (strchr(argv[optind], '+') != NULL) {
        char *files_copy = strdup(argv[optind]);
        char *file = strtok(files_copy, "+");
        while (file != NULL) {
            if (stat(file, statbuf) != 0) {
                perror("stat");
                free(files_copy);
                return CP_FAILURE;
            }
            file = strtok(NULL, "+");
        }
        free(files_copy);
    } else {
        if (stat(argv[optind], statbuf) != 0) {
            perror("stat");
            return CP_FAILURE;
        }
    }

    return CP_SUCCESS;
}

int mycp(int argc, char *argv[]) {
    struct stat statbuf;
    struct utimbuf timeby;
    int overwrite = 0, verify = 0;

    struct option long_options[] = {
            {"y", no_argument, &overwrite, 0},
            {0, 0, 0, 0}
    };

    while (1) {
        int opt = getopt_long(argc, argv, "yv", long_options, NULL);
        if (opt == -1) break;

        switch (opt) {
            case 'y':
                overwrite = 1;
                break;
            case 'v':
                verify = 1;
                break;
            case 0:
                overwrite = 0;
                break;
            default:
                print_usage_cp();
                return CP_FAILURE;
        }
    }

    if (check(argc, argv, &statbuf) != CP_SUCCESS) {
        return -1;
    }

    const char *src = argv[optind];
    const char *dest = argv[optind + 1];

    int result = mycp_main(src, dest, overwrite, verify);

    if (result == CP_SUCCESS) {
        stat(src, &statbuf);
        timeby.actime = statbuf.st_atime;
        timeby.modtime = statbuf.st_mtime;
        utime(dest, &timeby);

        printf("Copy Finished!\n");
    } else {
        printf("Copy Failed!\n");
    }

    printf("\n");
    return 1;
}
