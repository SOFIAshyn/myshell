#include "operations/operations.hpp"
#include <stdlib.h> // setenv

int mexport(const vector<string> &usr_commands, bool h_flag) {
    if (h_flag) {
        std::cout << "mexport usage : mexport var_name=VAL [-h|--help]" << std::endl;
        return 0;
    }
    if (usr_commands.size() != 2) {
        std::cout << "mexport: should be one argument provided.";
        return 1;
    }
    int status;

    auto var_val = usr_commands[1];
    if (var_val.find("=") == string::npos) {
        std::cout << "mexport: run: mexport var_name=VAL [-h|--help]" << std::endl;
        return 3;
    }
    string name = var_val.substr(0, var_val.find("="));
    string value = var_val.substr(var_val.find("=") + 1, var_val.size());
    status = setenv(name.c_str(), value.c_str(), 1);

    return status;
}