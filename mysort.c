#include "shell.h"

void print_usage_sort() {
    puts("Usage: mysort [options] [input_file] [/o output_file]");
    puts("Options:");
    puts("  -r      : 按降序排序");
    puts("  -+n     : 从第n列开始排序（列数从1开始计数）");
    puts("  -l[C]   : 指定排序使用的区域语言（C代表区分大小写）");
    puts("  -t [路径]: 指定排序时临时文件的路径");
    puts("  -R n    : 限制排序操作中可用的最大记录数");
}

const char* move_to_column(const char *str, int col) {
    return (strlen(str) < col - 1) ? "" : str + col - 1;
}

int compare(const void *a, const void *b, void *arg) {
    int start_col = *(int *)arg;
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;

    str1 = move_to_column(str1, start_col);
    str2 = move_to_column(str2, start_col);

    return strcmp(str1, str2);
}

int compare_desc(const void *a, const void *b, void *arg) {
    int start_col = *(int *)arg;
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;

    str1 = move_to_column(str1, start_col);
    str2 = move_to_column(str2, start_col);

    return strcmp(str2, str1);
}

int mysort(int argc, char *argv[]) {
    int desc = 0; // Default to ascending order
    int start_col = 1;
    int rec_limit = -1;
    char *locale = NULL;
    char *temp_path = NULL;

    struct option long_options[] = {
            {"+n", required_argument, NULL, '+'},
            {"rec", required_argument, NULL, 'R'},
            {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "r+::l::t::R:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'r':
                desc = 1;
                break;
            case '+':
                start_col = atoi(optarg);
                break;
            case 'l':
                locale = optarg;
                break;
            case 't':
                temp_path = optarg;
                break;
            case 'R':
                rec_limit = atoi(optarg);
                break;
            default:
                print_usage_sort();
                goto flagreturn;
                return (EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        print_usage_sort();
        exit(EXIT_FAILURE);
    }

    char *input_file = argv[optind];
    char *output_file = NULL;

    if (optind + 1 < argc) {
        if (strncmp(argv[optind + 1], "/o", 2) == 0) {
            output_file = argv[optind + 2];
        } else {
            output_file = argv[optind + 1];
        }
    }

    FILE *file = fopen(input_file, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char **lines = NULL;
    size_t line_count = 0;
    size_t line_capacity = 0;
    char buffer[BUFSIZ];

    while (fgets(buffer, sizeof(buffer), file)) {
        if (rec_limit != -1 && line_count >= rec_limit) {
            break;
        }
        if (line_count >= line_capacity) {
            line_capacity = line_capacity ? line_capacity * 2 : 1;
            lines = realloc(lines, line_capacity * sizeof(char *));
        }
        lines[line_count++] = strdup(buffer);
    }

    fclose(file);

    qsort_r(lines, line_count, sizeof(char *), desc ? compare_desc : compare, &start_col);

    FILE *output = (output_file) ? fopen(output_file, "w") : stdout;
    if (!output) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < line_count; ++i) {
        fprintf(output, "%s\n", lines[i]);
        free(lines[i]);
    }

    free(lines);

    if (output_file) {
        fclose(output);
    }
    flagreturn:
    return 0;
}
