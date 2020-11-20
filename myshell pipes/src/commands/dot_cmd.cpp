#include "operations/operations.hpp"
#include "operations/parse.h"
#include "operations/run_cmd.h"

int dot_cmd(const vector<string> &usr_commands, bool h_flag) {
    if (h_flag) {
        std::cout << "usage: . <name_of_script.msh> [-h|--help]" << std::endl;
        return 0;
    }
    if (usr_commands.size() != 2) {
        std::wcerr << ". : wrong number of arguments. use [-h|--help] to discover the usage." << std::endl;;
        return -1;
    }

    std::ifstream script(usr_commands[1]);
    string script_line;
    vector<string> line_usr_commands;
    int status;

    if (!script.is_open()) {
        script.close();
        return -2;
    }

    while (getline(script, script_line)) {
        if ((script_line.empty() || script_line[0] == '#')) {
            continue;
        }
        parse_usr_command(script_line, line_usr_commands);
        std::map<string, int> stream_;
        if (!line_usr_commands.empty())
            run_line(line_usr_commands, stream_, false, nullptr, nullptr);
        line_usr_commands.clear();
    }
    script.close();

    return status;
}