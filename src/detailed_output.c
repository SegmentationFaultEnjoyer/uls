#include "../inc/uls.h"

void detailed_ouput(char **files, int size, bool *flags, bool NeedTotal, bool name_flag) {
    bool was_device = false;
    int max_user_size = 0, max_gr_size = 0, max_byte_size = 0, blocks = 0, max_links_size = 0,
    max_inode_size = 0;
    struct stat buf;
    acl_t acl = NULL;
    ssize_t xatr_len, value_len;
    struct passwd *pw;
    struct group *group;
    long int ttime;
    char **temp = copy_strarr(files, size);
    char list[XATTR_SIZE], value[XATTR_SIZE];
    while(*temp != NULL) {
        lstat(*temp, &buf);
        if(NeedTotal) blocks += buf.st_blocks;
        pw = getpwuid(buf.st_uid);
        group = getgrgid(buf.st_gid);

        if(S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode)) was_device = true;

        if(flags[fl_i]) {
            max_inode_size = max_inode_size < mx_strlen(mx_itoa(buf.st_ino)) ? mx_strlen(mx_itoa(buf.st_ino)) : max_inode_size;
        }

        if(flags[fl_n]) max_user_size = max_user_size < mx_strlen(mx_itoa(buf.st_uid)) ? mx_strlen(mx_itoa(buf.st_uid)) : max_user_size;
        else max_user_size = max_user_size < mx_strlen(pw->pw_name) ? mx_strlen(pw->pw_name) : max_user_size;
       
        
        if(flags[fl_n] || group == NULL) max_gr_size = max_gr_size < mx_strlen(mx_itoa(buf.st_gid)) ? mx_strlen(mx_itoa(buf.st_gid)) : max_gr_size;
        else max_gr_size = max_gr_size < mx_strlen(group->gr_name) ? mx_strlen(group->gr_name) : max_gr_size;

        max_byte_size =  max_byte_size < mx_strlen(mx_itoa(buf.st_size)) ? mx_strlen(mx_itoa(buf.st_size)) : max_byte_size;
        max_links_size = max_links_size < mx_strlen(mx_itoa(buf.st_nlink)) ? mx_strlen(mx_itoa(buf.st_nlink)) : max_links_size;
        temp++;
    }
    temp -= (sizeof(char) * size);
    if(NeedTotal) {
        mx_printstr(mx_strjoin("total ", mx_itoa(blocks)));
        mx_printchar('\n');
    }

    while (*files != NULL) { //inode number
        lstat(*files, &buf);
        int temp_size;
        if(flags[fl_i]) {
            temp_size = max_inode_size - mx_strlen(mx_itoa(buf.st_ino));
            while (temp_size--) mx_printchar(' ');
            mx_printstr(mx_itoa(buf.st_ino));
            mx_printchar(' ');
        }
        if(S_ISDIR(buf.st_mode) && !S_ISSOCK(buf.st_mode)) //dir check
            mx_printchar('d');
        else if(S_ISREG(buf.st_mode)) //file check
            mx_printchar('-');
        else if(S_ISLNK(buf.st_mode)) //link check
            mx_printchar('l');
        else if(S_ISFIFO(buf.st_mode)) //pipe check
            mx_printchar('p');
        else if (S_ISCHR(buf.st_mode)) //symb device check
            mx_printchar('c');
        else if(S_ISBLK(buf.st_mode) && !S_ISSOCK(buf.st_mode)) { //block device check
            mx_printchar('b');
        }
        else if(S_ISSOCK(buf.st_mode)) { //socket check
            mx_printchar('s');
        }
        

        if(buf.st_mode & S_IRUSR) //read acces owner
            mx_printchar('r');
        else
            mx_printchar('-');

        if(buf.st_mode & S_IWUSR) //write acces owner
            mx_printchar('w');
        else
            mx_printchar('-');

        if(buf.st_mode & S_ISUID) { //user byt
            mx_printchar('s');
        }
        else {
            if(buf.st_mode & S_IXUSR) //exec acces owner
                mx_printchar('x');
            else
                mx_printchar('-');
        }

        if(buf.st_mode & S_IRGRP) //read acces group
            mx_printchar('r');
        else
            mx_printchar('-');

        if(buf.st_mode & S_IWGRP) //write acces group
            mx_printchar('w');
        else
            mx_printchar('-');
        
        if(S_ISGID & buf.st_mode) { //gr byt
            mx_printchar('s');
        }
        else {
            if(buf.st_mode & S_IXGRP) //exec acces group
                mx_printchar('x');
            else
                mx_printchar('-');
        }
        if(buf.st_mode & S_IROTH) //read acces other
            mx_printchar('r');
        else
            mx_printchar('-');

        if(buf.st_mode & S_IWOTH) //write acces other
            mx_printchar('w');
        else
            mx_printchar('-');
        
        if(S_ISVTX & buf.st_mode) { //sticky byt
            mx_printchar('t');
        }
        else {
            if(buf.st_mode & S_IXOTH) //exec acces other
                mx_printchar('x');
            else
                mx_printchar('-');
        }

        xatr_len = listxattr(*files, list, XATTR_SIZE, XATTR_NOFOLLOW); //detailed attribute 
        acl = acl_get_file(*files, ACL_TYPE_EXTENDED);
        if(xatr_len > 0)
            mx_printstr("@ ");
        else if(acl != NULL) 
            mx_printstr("+ ");
        else
            mx_printstr("  ");

        temp_size = max_links_size - mx_strlen(mx_itoa(buf.st_nlink));
        while (temp_size--) mx_printchar(' ');
        mx_printint(buf.st_nlink); //links number
        mx_printchar(' ');

         //user name
        if(!flags[fl_g]) {
            if(flags[fl_n]) {
                    if(buf.st_uid == 0) mx_printchar('0');
                    else mx_printint(buf.st_uid);
                    temp_size = max_user_size + 2 - mx_strlen(mx_itoa(buf.st_uid));
                    while(temp_size--) 
                        mx_printchar(' ');
                }
            else {
                pw = getpwuid(buf.st_uid);
                mx_printstr(pw->pw_name);
                temp_size = max_user_size + 2 - mx_strlen(pw->pw_name);
                while(temp_size--) 
                    mx_printchar(' ');
            }
        }

        if(!flags[fl_o]) {   //group owner name
            if(flags[fl_n]) {
                if(buf.st_gid == 0) mx_printchar('0');
                else mx_printint(buf.st_gid);
                temp_size = max_gr_size + 2 - mx_strlen(mx_itoa(buf.st_gid));
                while(temp_size--) 
                    mx_printchar(' ');
            }
            else {
                group = getgrgid(buf.st_gid);  
                if(group == NULL) {
                    mx_printstr(mx_itoa(buf.st_gid));
                    temp_size = max_gr_size + 2 - 4;
                }
                else {
                    mx_printstr(group->gr_name);
                    temp_size = max_gr_size + 2 - mx_strlen(group->gr_name);
                }
                while(temp_size--) 
                    mx_printchar(' ');
            }
        }
    if(flags[fl_o] && flags[fl_g]) mx_printstr("  ");
        if(S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode)) { //devices
            
            if (major(buf.st_rdev) == 0) {
                mx_printstr("  0");
            }
            else {
                temp_size = 3 - mx_strlen(mx_itoa(major(buf.st_rdev)));
                while(temp_size--) 
                    mx_printchar(' ');
                mx_printint(major(buf.st_rdev));
            }            
            mx_printchar(',');
            if (minor(buf.st_rdev) == 0) {
                mx_printstr("   0 ");
            }
            else {
                temp_size = 4 - mx_strlen(mx_itoa(minor(buf.st_rdev)));
                while(temp_size-- > 0) 
                    mx_printchar(' ');
                if(mx_strlen(mx_itoa(minor(buf.st_rdev))) > 3) { 
                    mx_printchar(' ');
                    mx_printstr(minor_to_hex(mx_nbr_to_hex(minor(buf.st_rdev))));
                }
                else mx_printint(minor(buf.st_rdev));
                mx_printchar(' ');
            }       
            
        }
        else {
            if (was_device) {
                    mx_printstr("       ");
                }
            if(flags[fl_h] && !was_device) {//file size
                int letter = 0;
                float temp_float = (float)buf.st_size;
                convert_size(&temp_float, &letter);
                int temp_int = (int) (temp_float * 10);
                if (temp_int == 0)
                    temp_size = 3 ;
                else if (letter != 0 && mx_strlen(mx_itoa(temp_int)) < 3) 
                    temp_size = 5 - (mx_strlen(mx_itoa(temp_int)) + 2);
                else if (letter != 0) 
                    temp_size = 2;
                else 
                    temp_size = 5 - mx_strlen(mx_itoa(temp_int));

                while(temp_size--)
                    mx_printchar(' ');
                if(temp_int == 0) mx_printstr("0");
                else mx_printint(temp_int / 10);
                if(letter != 0 && mx_strlen(mx_itoa(temp_int)) < 3) {
                    mx_printchar('.');
                    if(temp_int % 10 == 0) mx_printchar('0');
                    else mx_printint((temp_int % 10));
                }
                print_size_name(letter);
                mx_printchar(' ');
            }
            else {
                temp_size = max_byte_size - mx_strlen(mx_itoa(buf.st_size));
                while(temp_size--) 
                    mx_printchar(' ');
                if(buf.st_size == 0) mx_printchar('0');
                else mx_printint(buf.st_size); 
                mx_printchar(' ');
            }
        }

        if(flags[fl_c]) ttime = buf.st_ctimespec.tv_sec; //last state change
        else if(flags[fl_u]) ttime = buf.st_atimespec.tv_sec; //last acces
        else if(flags[fl_U]) ttime = buf.st_birthtimespec.tv_sec; //birthday
        else ttime = buf.st_mtimespec.tv_sec;                //last change

        char *tmp = ctime(&ttime); //format of date

        tmp += 4 * sizeof(char);
        if(flags[fl_T]) tmp = mx_strndup(tmp, 20);
        else if(time(NULL) - ttime > HALF_YEAR || time(NULL) - ttime < 0) {
            tmp = mx_strndup(tmp, 20);
            mx_printstr(mx_strndup(tmp, 7));
            tmp += 15 * sizeof(char);
        }
        else tmp = mx_strndup(tmp, 12);
        mx_printstr(tmp);
        mx_printchar(' ');
        if(time(NULL) - ttime > HALF_YEAR || time(NULL) - ttime < 0) tmp -= 15 * sizeof(char);
        if(tmp) mx_strdel(&tmp);
        
        if(flags[fl_G] && isatty(STDOUT_FILENO)) { //colorful output
            color_print(*files, name_flag);
        }
        else {
            mx_printstr(name_flag ? get_name(*files) : *files); //file name
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

        if(S_ISLNK(buf.st_mode)) {
            char *temp_buf = mx_strnew(1024);
            mx_printstr(" -> ");
            readlink(*files, temp_buf, 1024);
            mx_printstr(temp_buf);
            mx_strdel(&temp_buf);
        }

        mx_printchar('\n');

        if(xatr_len > 0 && flags[fl_dog]) { //show detailed atribute
            for(int i = 0; i < xatr_len; i += mx_strlen(&list[i]) + 1) {
                mx_printstr("\t");
                mx_printstr(&list[i]);
                mx_printstr("\t");
                value_len = getxattr(*files, &list[i], value, XATTR_SIZE, 0, 0);
                if(value_len <= 0) mx_printchar('0');
                else mx_printint(value_len);
                if(flags[fl_h]) mx_printchar('B');
                mx_printchar('\n');
            }
            
        }
        files++;
    }
    files -= (sizeof(char) * size);
    if(temp) mx_del_strarr(&temp);
}

