#include "../inc/uls.h"

bool check_link_dir(char *file) {
    struct stat buf;
    char *temp_buf_read = mx_strdup(file), *temp_buf_write = NULL; 
    do {
        temp_buf_write = mx_strnew(1024);
        readlink(temp_buf_read, temp_buf_write, 1024);
        lstat(temp_buf_write, &buf);
        mx_strdel(&temp_buf_read);
        temp_buf_read = mx_strdup(temp_buf_write);
        mx_strdel(&temp_buf_write);
        if((buf.st_mode & S_IFDIR) && (buf.st_mode & S_IXUSR)) return true;
        
    } while(S_ISLNK(buf.st_mode));
    return false;
}

void init_args(int argc, char *argv[], char ***files, char ***dirs, int *f_am, int *d_am, bool *flags, bool *error_flag) {
    struct stat buf;
    int size = get_args_size(argc, argv);
    char **args = malloc(sizeof(char*) * (size + 1));
    args[size] = NULL;
    int counter = 0;
    int res;
    for (int i = 1; i < argc; i++) {
        if(argv[i][0] != '-' || counter) {
            res = lstat(argv[i], &buf);
            args[counter] = mx_strdup(argv[i]);
            if(((S_ISDIR(buf.st_mode) && !S_ISSOCK(buf.st_mode)) 
            || (!flags[fl_l] && check_link_dir(argv[i]))) && res == 0) { //dir or file
                (*d_am)++;
            }
            else if((S_ISFIFO(buf.st_mode) || S_ISLNK(buf.st_mode) || S_ISREG(buf.st_mode)
            || (S_ISCHR(buf.st_mode)) || S_ISBLK(buf.st_mode) || (S_ISSOCK(buf.st_mode))) && res == 0) {
                if(mx_strcmp(argv[i], "uls") != 0)
                    (*f_am)++;
            }
            counter++; 
        }
    }
    mx_bubble_sort(args, size, def_comp);    
    (*files) = malloc(sizeof(char*) * ((*f_am) + 1));
    (*files)[*f_am] = NULL;
    (*dirs) = malloc(sizeof(char*) * ((*d_am) + 1));
    (*dirs)[*d_am] = NULL;
    for (int i = 0, i_f = 0, i_d = 0; i < size; i++) {
        res = lstat(args[i], &buf);
        if(((S_ISDIR(buf.st_mode) && !S_ISSOCK(buf.st_mode)) 
        || (!flags[fl_l] && check_link_dir(args[i]))) && res == 0) //dir or file
            (*dirs)[i_d++] = mx_strdup(args[i]);
        else if(((S_ISFIFO(buf.st_mode) || S_ISLNK(buf.st_mode) || S_ISREG(buf.st_mode) 
        || (S_ISCHR(buf.st_mode)) || S_ISBLK(buf.st_mode) || (S_ISSOCK(buf.st_mode))) && res == 0)) {
            if(mx_strcmp(args[i], "uls") != 0)
                (*files)[i_f++] = mx_strdup(args[i]);
        }
        else {
            *error_flag = true;
            perror((mx_strjoin("uls: ", args[i])));
        }
    }
    sort_args (files, *f_am, flags);
    sort_args (dirs, *d_am, flags);
    for(int i = 0; i < size; i++) {
        free(args[i]);
        args[i] = NULL;
    }
    free(args);
    args = NULL;
}

