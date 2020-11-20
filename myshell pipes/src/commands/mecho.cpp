#include "operations/operations.hpp"

int mecho(const vector<string> &usr_commands, bool h_flag) {
    if (h_flag) {
        std::cout << "mecho usage : mecho [-h|--help] [text|$<var_name>] [text|$<var_name>] ..." << std::endl;
        return 0;
    }
    int status = 0;
    string to_print;
    for (auto arg = usr_commands.begin() + 1; arg != usr_commands.end(); ++arg) {
        auto var_start = arg->find("$");
        if (var_start != string::npos) {
            string var_name = arg->substr(var_start + 1, arg->size());
            auto val_of_var = getenv(var_name.data());
            to_print += arg->substr(0, arg->find("$"));
            if (val_of_var != nullptr) {
                to_print = to_print + val_of_var;
                std::cout << "\n here:\n"; // << val_of_var; // xxx
            }
        } else {
            to_print = to_print + *arg;
            std::cout << "\nhere 2:\n"; // xxx
        }
        to_print += " ";
    }
    std::cout << to_print << std::endl;
    return status;
}