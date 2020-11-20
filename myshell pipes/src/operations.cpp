#include "operations/operations.hpp"

shellCommands::shellCommands() {
    add_cmd(".", dot_cmd);
    add_cmd("merrno", merrno);
    add_cmd("mpwd", mpwd);
    add_cmd("mcd", mcd);
    add_cmd("mexit", mexit);
    add_cmd("mecho", mecho);
    add_cmd("mexport", mexport);
}

void shellCommands::add_cmd(const string &name, myshellFunction func) {
    shellCommands::map_of_functions.insert(std::pair<string, myshellFunction>(name, func));
}

bool shellCommands::find_cmd(const string &name) {
    auto itr = map_of_functions.find(name);
    if (itr == map_of_functions.end()) {
        return false;
    } else {
        return true;
    }
}

myshellFunction shellCommands::get_function(const string &name) {
    if (shellCommands::find_cmd(name))
        return map_of_functions[name];
    return nullptr;
}
