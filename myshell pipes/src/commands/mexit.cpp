#include "operations/operations.hpp"

int mexit(const vector<string> &usr_commands, bool h_flag) {
    if (h_flag) {
        std::cout << "mexit usage : mexit [exit code] [-h|--help]" << std::endl;
        return 0;
    }
    if (usr_commands.size() > 2) {
        std::cout << "mexit: should be one or zero argument provided.";
        return 1;
    }
    if (usr_commands.size() == 2) {
        exit(std::stoi(usr_commands[1]));
    }
    exit(0);
}