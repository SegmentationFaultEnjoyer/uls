#include "../inc/uls.h"

void default_output(char **files, int size, bool *flags, bool total_flag, bool name_flag) {
    if (flags[fl_C]) print_tab(files, size, flags, name_flag);
    else if (flags[fl_1]) print_default(files, "\n", flags, name_flag);
    else if (flags[fl_l]) detailed_ouput(files, size, flags, total_flag, name_flag);
    else if (flags[fl_m]) print_default(files, ", ", flags, name_flag);
    else if(isatty(STDOUT_FILENO))
        print_tab(files, size, flags, name_flag);
    else
        print_default(files, "\n", flags, name_flag);        
}

