#ifndef MYSHELL_PARSE_H
#define MYSHELL_PARSE_H
#include <iostream>
#include <vector>
#include <map>
// parse the commands
#include <boost/algorithm/string.hpp>
// wildcards
#include <glob.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STDIN_NUM "0"
#define STDOUT_NUM "1"
#define STDERR_NUM "2"

using std::vector;
using std::string;
using boost::is_any_of;

void get_separate_commands(vector<string> &separated_commands, string usr_command_org);

void add_spaces_to_usr_command(string &usr_cmd, const size_t &pos_red, size_t &cut);

void redirection_command_parser(string &usr_cmd, const string symb);

void parse_usr_command(string &usr_cmd, vector<string> &usr_commands);

int wild_just_end_(const string& argum, size_t i);

void add_wildcards_to_command_(vector<string> &usr_commands);

vector<string> get_particular_wildcard(const string& pattern);

int check_for_the_redirection(vector<string> &usr_commands, std::map<string, int> &stream_);

bool set_env_var(const string &cmd, size_t& pos_of_env_var, string& var_name, string& var_cmd);

string get_env_var(const string &cmd, size_t& pos_of_env_var);

int check_for_env_variable(const string &usr_command_s, string& var_name);

bool check_for_the_background_exec(vector<string> &usr_commands);

#endif //MYSHELL_PARSE_H
