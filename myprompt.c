#include "shell.h"

void myprompt(int argc, char *arglist[], char *buffer, size_t buffer_size) {
    if (argc < 2) {
        fprintf(stderr, "Usage: myprompt <format>\n");
        buffer[0] = '\0'; // 清除缓冲区以指示错误
        return;
    }

    char *format = arglist[1];
    time_t t;
    struct tm *tm_info;
    int i, j;

    time(&t);
    tm_info = localtime(&t);

    for (i = 0, j = 0; format[i] != '\0' && j < buffer_size - 1; i++) {
        if (format[i] == '$') {
            i++;
            switch (format[i]) {
                case '$': buffer[j++] = '$'; break;
                case 'A': buffer[j++] = '&'; break;
                case 'B': buffer[j++] = '|'; break;
                case 'C': buffer[j++] = '('; break;
                case 'D': strftime(buffer + j, buffer_size - j, "%Y-%m-%d", tm_info); j += strlen(buffer + j); break;
                case 'E': buffer[j++] = 27; break;
                case 'F': buffer[j++] = ')'; break;
                case 'G': buffer[j++] = '>'; break;
                case 'H': if (j > 0) j--; break; // 退格
                case 'L': buffer[j++] = '<'; break;
                case 'N': buffer[j++] = 'A'; break; // 假设单驱动器环境
                case 'P': getcwd(buffer + j, buffer_size - j); j += strlen(buffer + j); break;
                case 'Q': buffer[j++] = '='; break;
                case 'S': buffer[j++] = ' '; break;
                case 'T': strftime(buffer + j, buffer_size - j, "%H:%M:%S", tm_info); j += strlen(buffer + j); break;
                case 'V': buffer[j++] = '1'; buffer[j++] = '.'; buffer[j++] = '0'; break; // 假设版本 1.0
                case '_': buffer[j++] = '\n'; break;
                default: buffer[j++] = format[i]; break;
            }
        } else {
            buffer[j++] = format[i];
        }
    }
    buffer[j] = '\0';
}
