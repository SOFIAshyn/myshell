#include <iostream>
#include <stdlib.h> // getenv
#include <boost/program_options.hpp>
#include <vector>
#include <string>
#include <map>
// readline will read a line from the terminal and return it
#include <readline/readline.h>
#include <readline/history.h>
// all shell functions
#include "operations/operations.hpp"
#include "operations/parse.h"
#include "operations/run_cmd.h"

extern shellCommands shellCommands;
__thread int status_merrno;

int main(int argc, char **argv) {

    if (argc > 1 && boost::filesystem::extension(argv[1]) == ".msh" ) {
        // vector (first & last)
        int status = shellCommands.get_function(".")(vector<string>(argv, argv + argc), false);
        status_merrno = status;
        return status;
    }

    bool close_shell = false;
    bool back_exec = false;
    char* usr_command_org;
    vector<string> usr_commands;

    while (! close_shell) {
        signal(SIGCHLD, SIG_IGN);

        // add new path to the files to execute
        string path_str = set_path();
        setenv("PATH", path_str.c_str(), 1); // change the env variable, with added path for the external commands

        // get cur dir path
        char path_dir[DIR_NAME_LEN];
        auto got_str = getcwd(path_dir, DIR_NAME_LEN); // copy cur path to char (from path_str to path_dir)
        if (got_str == nullptr)
            exit(EXIT_FAILURE);
        strcat(path_dir, " $ "); // set to have it in the shell, that we've created

        // start cmd-work
        usr_command_org = readline(path_dir);
        vector<string> separated_commands;
        if (usr_command_org) {
            add_history(usr_command_org);
            get_separate_commands(separated_commands, std::string(usr_command_org));
            // create 2d vector of pipes of size separated_commands
            int pipes[separated_commands.size()-1][2];

            for (size_t i = 0; i < separated_commands.size(); ++i) {
                if (separated_commands[i].find("close") != string::npos) {
                    close_shell = true;
                    usr_commands.clear();
                    return EXIT_SUCCESS;
                }

                // check var_name
                string var_name;
                if (int set_or_get = check_for_env_variable(separated_commands[i], var_name)) {
                    if (set_or_get == 0) {
                        // it's a command, there is no $
                        // do nothing
                    } else if (set_or_get == 1) {
                        // we set up an env var here
                        // it is set up in the following function
                        // next, we wanna run the command
                    }
                    if (set_or_get == 2 || set_or_get == 1) {
                        auto env_var_val = getenv(var_name.c_str());
                        if (env_var_val) {
                            // var is set and exists
                            separated_commands[i] = string(env_var_val);
                        }
                    }
                }

                parse_usr_command(separated_commands[i], usr_commands);
                // in *usr_commands* we have each command in separate
                if (!usr_commands.empty()) {
                    add_wildcards_to_command_(usr_commands);
                    // check for env variable
                    back_exec = check_for_the_background_exec(usr_commands);
                    // redirection
                    map<std::string, int> stream_;
                    check_for_the_redirection(usr_commands, stream_);
                    if (pipe(pipes[i]) == -1) {
                        std::cerr << "Error creating pipe.\n";
                        exit(EXIT_FAILURE);
                    }

                    if (i == 0) {
                        run_line(usr_commands, stream_, back_exec, nullptr, pipes[i]);
                    }
                    if (i == separated_commands.size() - 1) {
                        run_line(usr_commands, stream_, back_exec, pipes[i - 1], nullptr);
                    }
                    if (i > 0 && i < separated_commands.size() - 1) {
                        run_line(usr_commands, stream_, back_exec, pipes[i - 1], pipes[i]);
                    }

                    if (i > 0) {
                        close(pipes[i-1][0]);
                        close(pipes[i-1][1]);
                    }
                } else {
                    std::cerr << "Error: empty command line.\n";
                    continue;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

string set_path() {
    // add environment variable
    char *path_ptr = getenv("PATH");
    string path_str;
    if (path_ptr == nullptr)
        exit(EXIT_FAILURE); // the environment is not set up
    path_str = path_ptr;

    char path_dir[DIR_NAME_LEN];
    auto got_cwd = getcwd(path_dir, DIR_NAME_LEN); // copy cur path to string
    if (got_cwd == nullptr)
        exit(EXIT_FAILURE);
    path_str = path_str +  ":" + path_dir + "/external_cmds";
    return path_str;
}