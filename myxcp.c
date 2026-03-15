#include "shell.h"

#define CP_SUCCESS 1
#define CP_FAILURE 2

void print_usage_mxcp() {
    puts("Usage: myxcp [options] <source> <destination>");
    puts("Options:");
    puts(" -a : 仅复制有存档属性集的文件，但不更改属性。");
    puts(" -m : 仅复制有存档属性集的文件，并关闭存档属性。");
    puts(" -d[:日期] : 复制在指定日期或之后更改的文件。");
    puts(" -p : 复制每个文件前进行提示。");
    puts(" -s : 复制目录和子目录，但不复制空目录。");
    puts(" -e : 复制目录和子目录，包括空目录。");
    puts(" -v : 验证每个新文件是否正确。");
    puts(" -w : 提示你在复制前按下任意键。");
    puts(" -c : 忽略错误继续复制。");
    puts(" -i : 如果目标不存在且复制多个文件，假定目标必须是目录。");
    puts(" -q : 复制时不显示文件名。");
    puts(" -f : 复制时显示完整的源文件和目标文件名。");
    puts(" -l : 显示要复制的文件列表。");
    puts(" -g : 允许在不支持长文件名的情况下复制加密文件。");
    puts(" -h : 复制隐藏文件和系统文件。");
    puts(" -r : 复制只读文件。");
    puts(" -t : 仅复制目录结构（不复制文件）。");
    puts(" -u : 仅更新已经在目标上的文件。");
    puts(" -k : 复制文件和目录时保留属性。");
    puts(" -n : 使用生成的短文件名。");
}

int mxcp_copy_file(const char *src, const char *dest, int options[], time_t date) {
    struct stat st;
    if (lstat(src, &st) < 0) {
        perror("lstat");
        return CP_FAILURE;
    }
    if (S_ISDIR(st.st_mode)) {
        return mxcp_copy_directory(src, dest, options, date);
    }
    if (options[0] && !(st.st_mode & S_ISREG(st.st_mode))) {
        return CP_SUCCESS;
    }
    if (access(dest, F_OK) == 0 && !options[1]) {
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
    if (options[2]) {
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

int mxcp_copy_directory(const char *src, const char *dest, int options[], time_t date) {
    struct stat st;
    if (stat(src, &st) < 0) {
        perror("stat");
        return CP_FAILURE;
    }
    if (mkdir(dest, st.st_mode) < 0 && errno != EEXIST) {
        perror("mkdir");
        return CP_FAILURE;
    }
    DIR *dir = opendir(src);
    if (!dir) {
        perror("opendir");
        return CP_FAILURE;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char src_path[PATH_MAX];
        char dest_path[PATH_MAX];
        snprintf(src_path, PATH_MAX, "%s/%s", src, entry->d_name);
        snprintf(dest_path, PATH_MAX, "%s/%s", dest, entry->d_name);
        if (lstat(src_path, &st) < 0) {
            perror("lstat");
            closedir(dir);
            return CP_FAILURE;
        }
        if (S_ISDIR(st.st_mode)) {
            if (options[5]) {
                continue;
            }
            int res = mxcp_copy_directory(src_path, dest_path, options, date);
            if (res != CP_SUCCESS) {
                closedir(dir);
                return res;
            }
        } else {
            int res = mxcp_copy_file(src_path, dest_path, options, date);
            if (res != CP_SUCCESS) {
                closedir(dir);
                return res;
            }
        }
    }
    closedir(dir);
    return CP_SUCCESS;
}

int mxcp_concatenate_files(const char *files, const char *dest, int overwrite) {
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

int myxcp_check(int argc, char *argv[], struct stat *statbuf) {
    if (argc < 3) {
        print_usage_mxcp();
        return CP_FAILURE;
    }
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

int myxcp_main(const char *src, const char *dest, int options[], time_t date) {
    if (strchr(src, '+') != NULL) {
        return mxcp_concatenate_files(src, dest, options[1]);
    } else {
        return mxcp_copy_file(src, dest, options, date);
    }
}

int myxcp(int argc, char *argv[]) {
    struct stat statbuf;
    struct utimbuf timeby;
    int options[20] = {0}; // 扩展数组以支持更多选项
    time_t date = 0;
    struct option long_options[] = {
            {"a", no_argument, &options[0], 1},
            {"y", no_argument, &options[1], 1},
            {"v", no_argument, &options[2], 1},
            {"m", no_argument, &options[3], 1},
            {"p", no_argument, &options[4], 1},
            {"s", no_argument, &options[5], 1},
            {"e", no_argument, &options[6], 1},
            {"w", no_argument, &options[7], 1},
            {"c", no_argument, &options[8], 1},
            {"i", no_argument, &options[9], 1},
            {"q", no_argument, &options[10], 1},
            {"f", no_argument, &options[11], 1},
            {"l", no_argument, &options[12], 1},
            {"g", no_argument, &options[13], 1},
            {"h", no_argument, &options[14], 1},
            {"r", no_argument, &options[15], 1},
            {"t", no_argument, &options[16], 1},
            {"u", no_argument, &options[17], 1},
            {"k", no_argument, &options[18], 1},
            {"n", no_argument, &options[19], 1},
            {0, 0, 0, 0}
    };
    while (1) {
        int opt = getopt_long(argc, argv, "ayvmpsweciqflghrtukn", long_options, NULL);
        if (opt == -1) break;
        switch (opt) {
            case 'a': options[0] = 1; break;
            case 'y': options[1] = 1; break;
            case 'v': options[2] = 1; break;
            case 'm': options[3] = 1; break;
            case 'p': options[4] = 1; break;
            case 's': options[5] = 1; break;
            case 'e': options[6] = 1; break;
            case 'w': options[7] = 1; break;
            case 'c': options[8] = 1; break;
            case 'i': options[9] = 1; break;
            case 'q': options[10] = 1; break;
            case 'f': options[11] = 1; break;
            case 'l': options[12] = 1; break;
            case 'g': options[13] = 1; break;
            case 'h': options[14] = 1; break;
            case 'r': options[15] = 1; break;
            case 't': options[16] = 1; break;
            case 'u': options[17] = 1; break;
            case 'k': options[18] = 1; break;
            case 'n': options[19] = 1; break;
            case 'd':
                if (optarg) {
                    struct tm tm;
                    if (strptime(optarg, "%Y-%m-%d", &tm) == NULL) {
                        fprintf(stderr, "Invalid date format. Use YYYY-MM-DD.\n");
                        return CP_FAILURE;
                    }
                    date = mktime(&tm);
                }
                options[20] = 1;
                break;
            default:
                print_usage_mxcp();
                return CP_FAILURE;
        }
    }

    if (myxcp_check(argc, argv, &statbuf) != CP_SUCCESS) {
        return -1;
    }
    const char *src = argv[optind];
    const char *dest = argv[optind + 1];
    // 获取原文件和目标文件路径
    int result = myxcp_main(src, dest, options, date);
    if (result == CP_SUCCESS) {
        stat(src, &statbuf);
        timeby.actime = statbuf.st_atime;
        timeby.modtime = statbuf.st_mtime;
        utime(dest, &timeby);
        printf("Copy Finished!\n");
    } else {
        printf("Copy Failed!\n");
    }
    return 1;
}
