#include "shell.h"

void print_usage_type() {
    write(STDOUT_FILENO, "Usage: mytype [file1] [file2] ...\n", 35);
}

int mytype(int argc, char *argv[]) {
    if (argc < 2) { // 无参数
        print_usage_type();
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) { // 是否打开
            perror("open");
            continue;
        }

        char buffer[BUFSIZ];
        ssize_t bytes_read;
        while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
            if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
                perror("write");
                close(fd);
                return 1;
            }
        }

        if (bytes_read < 0) {
            perror("read");
        }

        close(fd);
    }

    return 0;
}
