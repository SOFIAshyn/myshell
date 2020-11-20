#include "../inc/operations/main.h"

extern int __thread status_merrno;

int merrno(const vector<string> &usr_commands, bool h_flag) {
    if (h_flag) {
        std::cout << "merrno usage : merrno [-h|--help]" << std::endl;
        return 0;
    }
    if (usr_commands.size() > 1) {
        std::cout << "merrno: should be one argument provided.";
        return 1;
    }
    std::cout << status_merrno << std::endl;
    return 0;
}