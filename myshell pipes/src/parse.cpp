#include "operations/parse.h"

void get_separate_commands(vector<string> &separated_commands, string usr_command_org) {
    const string symb = "|";

    boost::split(separated_commands, usr_command_org, boost::is_any_of(symb));
}

void add_spaces_to_usr_command(string &usr_cmd, const size_t &pos_red, size_t &cut) {
    if (usr_cmd[pos_red - 1] == '&') {
        usr_cmd.insert(pos_red - 1, " ");
        usr_cmd.insert((pos_red + 1) + 1, " ");
        cut = pos_red + 2;
    } else if (usr_cmd[pos_red + 1] == '&') {
        usr_cmd.insert(pos_red, " ");
        usr_cmd.insert((pos_red + 1) + 2, " ");
        cut = pos_red + 3;
    } else {
        usr_cmd.insert(pos_red, " "); // if was 2, in palce 2 we inserted " "
        usr_cmd.insert((pos_red + 1) + 1, " "); // now pos_red += 1, and one more cell further
        cut = pos_red + 2;
    }
}

void redirection_command_parser(string &usr_cmd, const string symb) {
    size_t cut = 0;
    auto is_redirections = usr_cmd.substr(cut, usr_cmd.length()).find(symb);
    size_t pos_red = 1;
    while (is_redirections != std::string::npos) {
        pos_red = is_redirections + cut;

        add_spaces_to_usr_command(usr_cmd, pos_red, cut);
        is_redirections = usr_cmd.substr(cut, usr_cmd.length()).find(symb);
    }
}

/*
 * Get user command and split the arguments with a space sign
 * Return nothing, use &
 */
void parse_usr_command(string &usr_cmd, vector<string> &usr_commands) {
    // ignore everything after #
    usr_cmd = usr_cmd.substr(0, usr_cmd.find("#"));

    redirection_command_parser(usr_cmd, ">");
    redirection_command_parser(usr_cmd, "<");

    boost::algorithm::split(usr_commands, usr_cmd, is_any_of(" "));

    for (auto cmd = usr_commands.begin(); cmd != usr_commands.end();) {
        if (cmd->find_first_not_of(' ') == string::npos)
            cmd = usr_commands.erase(cmd);  // return the location of the last element erased, what was " "
        else {
            ++cmd;
        }
    }
}

/*
 * Function clears up the vector of user command with no wildcards.
 *
 * return: vector usr_commands with the added wildcard filenames,
 *          deleted wildcards from the vector
 * */
void add_wildcards_to_command_(vector<string> &usr_commands) {
    vector<string> tmp_get_filenames;
    std::map<int, vector<string>> pos_filenames;

    int find_wildcards = 0;
    size_t counter = 0;

    for (size_t i = 0; i < usr_commands.size(); ++i) {
        find_wildcards = wild_just_end_(usr_commands[i], i); // if valid wildcard

        // if valid
        if (find_wildcards == 1) {

            tmp_get_filenames = get_particular_wildcard(usr_commands[i]);
            if (!tmp_get_filenames.empty()) {
                pos_filenames[counter] = tmp_get_filenames;
            } else {
                // there is no files with a particular wildcard
                std::wcerr << "warning: the wrond wildcard is used. there are no such files." << std::endl;
            }
        } else if (find_wildcards == -1) {
            std::cerr << "Error: unexpected symbol in the program name\n";
        } else if (find_wildcards == 0) {
            std::wcerr << "warning: wrong wildcard is used. don't use wildcards for the directories." << std::endl;
        }
        ++counter;
    }
    vector<string>().swap(tmp_get_filenames);

    for (auto const& [key, val] : pos_filenames) {
        usr_commands.insert(usr_commands.begin() + key, val.begin(), val.end());
    }

    counter = 0;
    for (auto cmd = usr_commands.begin(); cmd != usr_commands.end();) {
        if (usr_commands[counter].find('*') != string::npos) {
            cmd = usr_commands.erase(cmd);
            --counter;
        } else {
            ++cmd;
        }
        ++counter;
    }
}

/*
 * Wildcards should not be parsed everywhere
 * Return:
 *      1 - if a wildcard just at the end,
 *      0 - if a wildcard is in the middle of the path,
 *      -1 - if a wildcard is in the program name,
 *      2 - if there is no a wildcard
 * */
int wild_just_end_(const string& argum, size_t i) {
    vector<string> splitted_argum;

    boost::algorithm::split(splitted_argum, argum, is_any_of("/"));

    for (string path_part : splitted_argum) {
        if (path_part.find("*") != string::npos) {
            // if found
            if (splitted_argum.back() != path_part) {
                // at the middle of the path or in the program name
                return 0;
            } else if (splitted_argum.front() == path_part && i == 0) {
                return -1;
            } else
                return 1;
        }
    }

    return 2;
}

/*
 * Find wildcards
 * Return: vector of filenames due to a wildcard
 */
vector<string> get_particular_wildcard(const string& pattern) {
    glob_t glob_res;
    memset(&glob_res, 0, sizeof(glob_res));

    int ret_val = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_res);
    if (ret_val != 0) {
        globfree(&glob_res);
        return vector<string>();
    }

    // collect the filenames
    vector<string> filenames;
    for (size_t i = 0; i < glob_res.gl_pathc; ++i) {
        filenames.push_back(string(glob_res.gl_pathv[i]));
    }

    // cleanup
    globfree(&glob_res);

    return filenames;
}


int check_for_the_redirection(vector<string> &usr_commands, std::map<string, int> &stream_) {
    size_t index_redirect = 0;

    for (size_t i = 0; i < usr_commands.size(); ++i) {
        if (i > 0 && i < usr_commands.size() - 1) {
            if (usr_commands[i] == ">") {
                if (index_redirect > i || index_redirect == 0) index_redirect = i;
                stream_[usr_commands[i + 1]] = STDOUT_FILENO;
            } else if (usr_commands[i] == "<") {
                if (index_redirect > i || index_redirect == 0) index_redirect = i;
                stream_[usr_commands[i + 1]] = STDIN_FILENO;
            } else if (usr_commands[i] == ">&") {
                if (index_redirect > i || index_redirect == 0) index_redirect = i;
                if ((usr_commands[i - 1] == STDERR_NUM) && (usr_commands[i + 1] == STDOUT_NUM)) {
                    dup2(STDOUT_FILENO, STDERR_FILENO);
                }
            }
        }
        if (usr_commands[i] == "&>" && i != 0) {
            if (index_redirect > i || index_redirect == 0) index_redirect = i;
            dup2(STDOUT_FILENO, STDERR_FILENO);
        }
    }

    // remove redirection signs
    if (index_redirect > 0) {
        usr_commands.erase(usr_commands.begin() + index_redirect, usr_commands.end());
    }

    return EXIT_SUCCESS;
}

bool set_env_var(const string &cmd, size_t& pos_of_env_var, string& var_name, string& var_cmd) {

    // if set
    auto eq = cmd.find('=');
    auto first_space = cmd.find(' ');
    if (first_space < eq && first_space != 0) eq = first_space;
    auto first_brace = cmd.find('(');
    if ((eq != std::string::npos) && (first_brace != std::string::npos)) {
        size_t ending_brace = cmd.find(")");
        if (ending_brace == std::string::npos) {
            std::cout << "Error: The command for the variable is not well specified.\n";
            return false;
        }
        // cut out the command
        var_name = cmd.substr(0, eq);
        // without '(' and ending_barce without ')' and '('
        var_cmd = cmd.substr(first_brace + 1, ending_brace - first_brace - 1);
        return true;
    } else {
        std::cout << "Error: The command for the variable is not well specified.\n";
        return false;
    }
}

string get_env_var(const string &cmd, size_t& pos_of_env_var) {
    auto end = cmd.find(" ");
    if (end == std::string::npos) {
        end = cmd.length();
    }
    if (pos_of_env_var + 1 >= end)
        return "";
    auto name = cmd.substr(pos_of_env_var + 1, end);

    return name;
}

int check_for_env_variable(const string &usr_command_s, string &var_name) {
    size_t pos_of_env_var = usr_command_s.find("$");
    if (pos_of_env_var == std::string::npos) {
        return 0;
    }
    // if set
    string var_cmd;
    char *env_var_cmd;
    if (set_env_var(usr_command_s, pos_of_env_var, var_name, var_cmd)) {
         std::cout << "var_name = " << var_name << " ; var_cmd = " << var_cmd << "\n";
         if (setenv(var_name.c_str(), var_cmd.c_str(), 1) == -1) {
             std::cout << "Error while setting the environment variable.\n";
             return -1;
         }
        return 1;
    } else {
        var_name = get_env_var(usr_command_s, pos_of_env_var);
        return 2;
    }
}

bool check_for_the_background_exec(vector<string>  &usr_commands) {
    if (usr_commands.size() > 1  && usr_commands[usr_commands.size() - 1] == "&" ) {
        usr_commands.pop_back();
        return true;
    }
    return false;
}
