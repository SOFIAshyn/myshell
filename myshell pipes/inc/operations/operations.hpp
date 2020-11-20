#ifndef SHELL_OPERATIONS_HPP
#define SHELL_OPERATIONS_HPP
#include "operations/main.h"
//wait child proccess
#include <sys/types.h>
#include <sys/wait.h>
// exec
#include <unistd.h>
// ifstream
#include <fstream>
// extension
#include <boost/filesystem.hpp>


typedef int (*myshellFunction)(const vector<string> &, bool);

// name of function, its code to execute
typedef std::unordered_map<string, myshellFunction> commands_map;

int dot_cmd(const vector<string> &usr_commands, bool h_flag);

//int run_line(vector<string> &line_usr_comands);

int merrno(const vector<string> &usr_commands, bool h_flag);

int mpwd(const vector<string> &usr_commands, bool h_flag);

int mcd(const vector<string> &usr_commands, bool h_flag);

int mexit(const vector<string> &usr_commands, bool h_flag);

int mecho(const vector<string> &usr_commands, bool h_flag);

int mexport(const vector<string> &usr_commands, bool h_flag);

struct shellCommands {
    shellCommands();

    myshellFunction get_function(const string &name);

private:
    commands_map map_of_functions;
    bool find_cmd(const string &name);

    // replace **int(*cmds) (const vector<string> &, bool)** with **typedef myshellFunction**
    void add_cmd(const string &name, myshellFunction cmds);
};

#endif //ADDER_OPERATIONS_HPP
