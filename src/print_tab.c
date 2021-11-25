#include "../inc/uls.h"

void print_tab (char **files, int size, bool *flags, bool name_flag) {
    struct winsize w;
    struct stat buf;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    int size_of_terminal = w.ws_col;
    int tab_length = 8;
    int max_size = 0;
    for (int i = 0; i < size; i++) {
        if (max_size < mx_strlen(name_flag ? get_name(files[i]) : files[i]))
            max_size = mx_strlen(name_flag ? get_name(files[i]) : files[i]);
    }
    if (flags[fl_i]) max_size += 7;
    max_size += tab_length - (max_size % tab_length);
    int cols = size_of_terminal >= max_size ? size_of_terminal / max_size : 1;
    int rows = size / cols + (size % cols == 0 ? 0 : 1);
    char ***tab_matrix = malloc(sizeof(char**) * rows);
    int k = 0;
    for (int i = 0; i < rows; i++) {
        tab_matrix[i] = malloc(sizeof(char*) * cols);
    }
    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            if (k < size) {
                tab_matrix[i][j] = mx_strdup(files[k]);
                k++;
            }
            else tab_matrix[i][j] = NULL;
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (tab_matrix[i][j] != NULL) {
                lstat(tab_matrix[i][j], &buf);
                if (flags[fl_i]) {
                    mx_printint(buf.st_ino);
                    mx_printchar(' ');
                }
                if(flags[fl_G] && isatty(STDOUT_FILENO)) { //colorful output
                    color_print(tab_matrix[i][j], name_flag);
                }
                else {
                    mx_printstr(name_flag ? get_name(tab_matrix[i][j]) : tab_matrix[i][j]);
                }

                int temp = max_size - mx_strlen(name_flag ? get_name(tab_matrix[i][j]) : tab_matrix[i][j]);
                if((flags[fl_p] || flags[fl_F]) && S_ISDIR(buf.st_mode) && !S_ISSOCK(buf.st_mode)) {
                    mx_printchar('/');
                    temp--;
                }                
                if(flags[fl_F]) {
                    if(S_ISLNK(buf.st_mode) && temp--) mx_printchar('@');
                    else if((buf.st_mode & S_IXUSR) && S_ISREG(buf.st_mode) && !S_ISSOCK(buf.st_mode) && temp--) mx_printchar('*');
                    else if(S_ISSOCK(buf.st_mode) && temp--) mx_printchar('=');
                    else if(S_ISFIFO(buf.st_mode) && temp--) mx_printchar('|');
                }
                if (flags[fl_i]) temp -= 7;
                if (j != cols - 1 && tab_matrix[i][j + 1] != NULL) {
                    while (temp > 0) {
                        mx_printchar('\t');
                        temp -= tab_length;
                    }   
                }           
            }
        }
        mx_printchar('\n');
    }
    if(tab_matrix) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {            
                if (tab_matrix[i][j]) {
                    free(tab_matrix[i][j]);
                    tab_matrix[i][j] = NULL;
                }
            }
            if (tab_matrix[i]) {
                free(tab_matrix[i]);
                tab_matrix[i] = NULL;
            }
        }
        if (tab_matrix) {
            free(tab_matrix);
            tab_matrix = NULL;
        }
    }
}
