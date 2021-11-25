#include "../inc/uls.h"

void uls_print (int dir_am, bool *flags, char *dirs) {
    struct stat buf;
    char **dir_contains;
    int dir_cont_am = 0;
    lstat(dirs, &buf);
    dir_contains = init(&dir_cont_am, flags, dirs);
    if(dir_contains) {
        fix_path(&dir_contains, dirs, dir_cont_am);
        sort_args(&dir_contains, dir_cont_am, flags);
        if(mx_strcmp(dirs, ".") != 0 && dir_am > 1) { 
            mx_printstr(dirs);
            mx_printstr(":\n");
        }
        if(dir_contains[0] == NULL) default_output(dir_contains, dir_cont_am, flags, false, true);
        else default_output(dir_contains, dir_cont_am, flags, true, true);
        if (flags[fl_R]) {
            for (int j = 0; j < dir_cont_am; j++) {
                lstat(dir_contains[j], &buf);
                if (((buf.st_mode & S_IFDIR) && (buf.st_mode & S_IXUSR) && !S_ISSOCK(buf.st_mode)) && mx_strcmp(get_name(dir_contains[j]), ".") != 0 && mx_strcmp(get_name(dir_contains[j]), "..") != 0) {
                    mx_printchar('\n');
                    uls_print (++dir_am, flags, dir_contains[j]);
                }
            }
        }
        if(dir_contains) freeding(dir_contains);
        dir_cont_am = 0;
    }
    
}



