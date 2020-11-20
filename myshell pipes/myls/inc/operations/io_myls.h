#include <iostream>
#include <sstream>
#include <glob.h>
#include <sys/stat.h>
#include <string.h>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>

using std::string;
using std::vector;
using std::map;
using boost::is_any_of;

struct Options {
    bool use_h;
    bool use_l;
    bool use_U;
    bool use_S;
    bool use_t;
    bool use_X;
    bool use_N;
    bool use_D;
    bool use_s;
    bool use_r;
    bool use_F;
    bool use_R;
    vector<string> atribute_path;
};


struct Options get_ops(int argc, char* argv[], string &files);

void check_files_on_existance(vector<string> &ls_files, vector<string> &ls_dirs);

bool N_compare(string name1, vector<string> params1, string name2, vector<string> params2);

bool S_compare(string name1, vector<string> params1, string name2, vector<string> params2);

bool t_compare(string name1, vector<string> params1, string name2, vector<string> params2);

bool X_compare(string name1, vector<string> params1, string name2, vector<string> params2);

bool U_compare(string name1, vector<string> params1, string name2, vector<string> params2);

void options_runner_files(Options &opts, vector<string> &ls_files);

int wild_just_end(const string& argum);

void add_wildcards_to_command(vector<string> &usr_commands);

vector<string> get_particular_wildcard(const string& pattern);

void get_details(string &f_name, map<string, vector<string>> &map_all_files, string cur_dir);

bool is_dir(const char *path);

bool is_spec_file(const char*path);

vector<string> bubble_sort(bool (*comparison)(string, vector<string>, string, vector<string>),
                           map<string, vector<string>> params, vector<string> args, Options &opts);

void show_folder_files(string path, Options &opts);
