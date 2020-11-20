#include "operations/operations.hpp"

extern int __thread status_merrno;

int mcd(const vector<string> &usr_commands, bool h_flag) {
    if (h_flag) {
        std::cout << "mcd usage : mcd <path> [-h|--help]" << std::endl;
        return 0;
    }
    if (usr_commands.size() > 2) {
        std::cout << "mcd: too many arguments.";
        return 1;
    }
    if (usr_commands.size() < 2) {
        std::cout << "mcd: must contain two arguments.";
        return 2;
    }
    int status;
    if (usr_commands[1] == "~") {
        status = chdir(getenv("HOME"));
        if (status != 0)
            std::cout << "mcd: There is no such file or directory." << std::endl;
        return status;
    }
    status = chdir(usr_commands[1].c_str());
    if (status != 0)
        std::cout << "mcd: There is no such file or directory." << std::endl;

    return status;
}