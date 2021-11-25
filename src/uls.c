#include "../inc/uls.h"

bool was_error;

int main(int argc, char *argv[]) {
    char **files;
    char **dirs;
    bool error_flag = false;
    bool *flags = init_flags(argc, argv);
    int dir_am = 0, files_am = 0;
    init_args(argc, argv, &files, &dirs, &files_am, &dir_am, flags, &error_flag);
    if (files[0] == NULL && dirs[0] == NULL && !error_flag) {
        dir_am = 1;
        free(dirs);
        dirs = malloc(sizeof(char*) * 2);
        dirs[0] = mx_strdup(".");
        dirs[1] = NULL;
    }
    if (files[0]) { 
        default_output(files, files_am, flags, false, false);
    }
    for (int i = 0; i < dir_am; i++) {
        if(files[0] || i) mx_printchar('\n');
        if(files[0]) 
            uls_print(2, flags, dirs[i]);
        else uls_print(dir_am, flags, dirs[i]);
    }
    if(files) mx_del_strarr(&files);
    if(dirs) mx_del_strarr(&dirs);
    free(flags);
    flags = NULL;
    return was_error ? 1 : 0;
}
