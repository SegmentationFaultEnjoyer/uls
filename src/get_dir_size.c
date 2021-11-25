#include "../inc/uls.h"

bool was_error = false;

int get_dir_size(char *path, bool *flags) {
    int size = 0;
    DIR *dir;
    struct dirent *entry;
    dir = opendir(path);
    if(!dir) {
        mx_printerr("uls: ");
        mx_printerr(get_name(path));
        mx_printerr(": ");
        mx_printerr(strerror(errno));
        mx_printerr("\n");
        was_error = true;
        return 0;
    }
    while((entry = readdir(dir)) != NULL) {
        if((flags[fl_A] && mx_strcmp(entry->d_name, ".") != 0 && mx_strcmp(entry->d_name, "..") != 0 ) ||
            flags[fl_a] || entry->d_name[0] != '.')
        size++;
    }
    closedir(dir);
    return size;
}
