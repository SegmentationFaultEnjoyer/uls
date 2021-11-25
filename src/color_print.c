#include "../inc/uls.h"

static void print_with_back(char *back_color, char *text_color, char *text) {
    mx_printstr(back_color);
    mx_printstr(text_color);
    mx_printstr(text);
    mx_printstr(RESET_COLOR);
}

static void print_without_back(char *text_color, char *text) {
    mx_printstr(text_color);
    mx_printstr(text);
    mx_printstr(RESET_COLOR);
}

void color_print(char *str, bool name_flag) {
    struct stat buf;
    lstat(str, &buf);
    if(S_ISGID & buf.st_mode) //byt set-group-ID
        print_with_back(BLUE_BACK, BLACK_COLOR, name_flag ? get_name(str) : str);
    else if(buf.st_mode & S_ISUID) //byt set-user-ID
        print_with_back(RED_BACK, BLACK_COLOR, name_flag ? get_name(str) : str);
    else {
        if(S_ISDIR(buf.st_mode) && !S_ISSOCK(buf.st_mode)) { //dirs
            if((buf.st_mode & S_IWOTH) && (S_ISVTX & buf.st_mode)) //writable to others with sticky bit
                print_with_back(GREEN_BACK, BLACK_COLOR, name_flag ? get_name(str) : str);
            else if((buf.st_mode & S_IWOTH) && !(S_ISVTX & buf.st_mode)) //writable to others without sticky bit
                print_with_back(YELLOW_BACK, BLACK_COLOR, name_flag ? get_name(str) : str);
            else                                                        //regular dir
                print_without_back(BLUE_COLOR, name_flag ? get_name(str) : str);
        }
        else if(S_ISLNK(buf.st_mode)) //link
            print_without_back(PURPLE_COLOR, name_flag ? get_name(str) : str);
        else if ((buf.st_mode & S_IXUSR) && S_ISREG(buf.st_mode) && !S_ISSOCK(buf.st_mode)) //executable
            print_without_back(RED_COLOR, name_flag ? get_name(str) : str);
        else if (buf.st_mode & S_IFIFO) //pipe
            print_without_back(YELLOW_COLOR, name_flag ? get_name(str) : str);
        else if(S_ISCHR(buf.st_mode)) //symb dev
            print_with_back(YELLOW_BACK, BLUE_COLOR, name_flag ? get_name(str) : str);
        else if(S_ISBLK(buf.st_mode) && !S_ISSOCK(buf.st_mode)) //block dev
            print_with_back(BLUE_BACK, BLUE_COLOR, name_flag ? get_name(str) : str);
        else if(S_ISSOCK(buf.st_mode)) //socket
            print_without_back(GREEN_COLOR, name_flag ? get_name(str) : str);
        else //reg file
            mx_printstr(name_flag ? get_name(str) : str);
    }
}
