#include "operations/operations.hpp"
#include "operations/main.h"
#include <unistd.h>

int mpwd(const vector<string> &usr_commands, bool h_flag) {
    if (h_flag) {
        std::cout << "mpwd usage : mpwd [-h|--help]" << std::endl;
        return 0;
    }
    if (usr_commands.size() > 1) {
        std::cout << "mpwd: should be one argument provided.";
        return 1;
    }
    char path_dir[DIR_NAME_LEN];
    auto got_path = getcwd(path_dir, DIR_NAME_LEN); // copy cur path to string
    if (got_path == nullptr)
        return 2;
    std::cout << path_dir << std::endl;
    return 0;
}