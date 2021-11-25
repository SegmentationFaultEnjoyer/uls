#include "../inc/uls.h"

void print_default(char **arr, char *delim, bool *flags, bool name_flag) {
    struct winsize w;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    int size = 0;
    int term_size;
    term_size = isatty(STDOUT_FILENO) ? w.ws_col : 81;
    struct stat buf;
    while(*arr != NULL) {
        lstat(*arr, &buf);
        if (flags[fl_m]) {
            size += mx_strlen(get_name(*arr)) + 2;
            if((flags[fl_p] || flags[fl_F]) && S_ISDIR(buf.st_mode) && !S_ISSOCK(buf.st_mode)) {
                size++;
            }
            if(flags[fl_F]) {
                if(S_ISLNK(buf.st_mode)) size++;
                else if((buf.st_mode & S_IXUSR) && S_ISREG(buf.st_mode) && !S_ISSOCK(buf.st_mode)) size++;
                else if(S_ISSOCK(buf.st_mode)) size++;
                else if(S_ISFIFO(buf.st_mode)) size++;
            }
            if (flags[fl_i]) size += 7;
            if (size >= term_size && *arr) {
                mx_printchar('\n');
                size = mx_strlen(name_flag ? get_name(*arr) : *arr) + 2;
                if (flags[fl_i]) size += 7;
            }
        }
        if (flags[fl_i]) {
            mx_printint(buf.st_ino);
            mx_printchar(' ');
        }
        if(flags[fl_G] && isatty(STDOUT_FILENO)) { //colorful output
            color_print(*arr, name_flag);
        }
        else {
            mx_printstr(name_flag ? get_name(*arr) : *arr); //file name
        }
        if((flags[fl_p] || flags[fl_F]) && S_ISDIR(buf.st_mode) && !S_ISSOCK(buf.st_mode)) {
            mx_printchar('/');
        }
        if(flags[fl_F]) {
            if(S_ISLNK(buf.st_mode)) mx_printchar('@');
            else if((buf.st_mode & S_IXUSR) && S_ISREG(buf.st_mode) && !S_ISSOCK(buf.st_mode)) mx_printchar('*');
            else if(S_ISSOCK(buf.st_mode)) mx_printchar('=');
            else if(S_ISFIFO(buf.st_mode)) mx_printchar('|');
        }
        arr++;
        if (*arr)
            mx_printstr(delim);
        else mx_printchar('\n');

        
    }
}


