#include <dirent.h>
#include "../inc/operations/io_myls.h"


#define BUF_SIZE 1024
char compile_dir[BUF_SIZE];
const char* cur_dir = ".";

static void init_opts(struct Options* opts) {
    opts->use_h = false;
    opts->use_l = false;
    opts->use_U = false;
    opts->use_S = false;
    opts->use_t = false;
    opts->use_X = false;
    opts->use_N = true;
    opts->use_D = false;
    opts->use_s = false;
    opts->use_r = false;
    opts->use_F = false;
    opts->use_R = false;
}


struct Options get_ops(int argc, char* argv[], string &files) {
    struct Options opts;
    init_opts(&opts);
    int opt;
    const string sort_options = "U, S, t, X, N, D, s";
    const string options = "l, r, F, R";


    if (getcwd(compile_dir, BUF_SIZE) == nullptr) {
        std::cerr << "getcwd() error";
        exit(1);

    }

    std::ostringstream oss;
    std::ostringstream oss_files;
    bool files_flag = false;
    for (int i = 1; i < argc; ++i) {
        if ((! boost::algorithm::starts_with(argv[i], "-")) || files_flag) {
            // file after -- ot without -
            string file_to_add = argv[i];
            if (boost::algorithm::starts_with(argv[i], "\"") && boost::algorithm::ends_with(argv[i], "\"")) {
                file_to_add = file_to_add.substr(1, file_to_add.length()-2);
            }
            // if after --
            if (boost::algorithm::starts_with(argv[i], "-")) {
                file_to_add = file_to_add.substr(1, file_to_add.length()-1);
            }
            oss_files << file_to_add << " ";
        } else if (string(argv[i]) == "--") {
            // after this -- we have files
            files_flag = true;
        } else if (boost::algorithm::contains(argv[i], "sort")) {
            // sort options
            vector<string> strs;
            boost::split(strs, argv[i], boost::is_any_of("="));
            for (auto flag : strs[1]) {
                oss << flag;
            }
        } else if (boost::algorithm::contains(argv[i], "help")) {
            oss << "h";
        } else {
            // other options (flags)
            vector<string> strs;
            boost::split(strs, argv[i], boost::is_any_of("-"));
            oss << strs[1];
        }
    }
    files = oss_files.str();
    string arguments = oss.str();
    for (auto arg : arguments) {
        switch (arg) {
            case 'l': opts.use_l = true; break;
            case 'h': opts.use_h = true; break;
            case 'U': opts.use_U = true; break;
            case 'S': opts.use_S = true; break;
            case 't': opts.use_t = true; break;
            case 'X': opts.use_X = true; break;
            case 'N': opts.use_N = true; break;
            case 'D': opts.use_D = true; break;
            case 's': opts.use_s = true; break;
            case 'r': opts.use_r = true; break;
            case 'F': opts.use_F = true; break;
            case 'R': opts.use_R = true; break;
            default: std::cerr << "Wrong options provided." << std::endl; exit(EXIT_FAILURE);
        }
    }

    if (opts.use_h) {
        std::cout << "Usage myls : myls [path|mask] [-l] [-h|--help] [--sort=U|S|t|X|D|s] [-r]\n"
                     "List information about the FILEs (the current directory by default).";
        exit(EXIT_SUCCESS);
    }

    return opts;
}


/*
 * Prepare ls_files not to have any of not valid paths
 */
void check_files_on_existance(vector<string> &ls_files, vector<string> &ls_dirs) {
    struct stat buffer;
    vector<string> to_be_removed;
    if (ls_files.size() == 1) {
        ls_dirs.emplace_back(".");
    }
    for (auto &path : ls_files) {
        if (path == "")
            continue;
        if (stat (path.c_str(), &buffer) != 0) {
            // don't exist
            std::cerr << "myls: " << path << ": No such file or directory" << std::endl;
            to_be_removed.emplace_back(path);
        }
        if (S_ISDIR(buffer.st_mode))
            ls_dirs.emplace_back(path);
    }
    for (auto &rem_el : to_be_removed) {
        ls_files.erase(std::remove(ls_files.begin(), ls_files.end(), rem_el), ls_files.end());
    }
}

bool N_compare(string name1, vector<string> params1, string name2, vector<string> params2) {
    return name1.compare(name2) > 0;
}

bool S_compare(string name1, vector<string> params1, string name2, vector<string> params2) {
    return atoi(params1[1].c_str()) > atoi(params2[1].c_str());
}

bool t_compare(string name1, vector<string> params1, string name2, vector<string> params2) {
    auto f_time1 = params1[2];
    auto f_time2 = params2[2];
    time_t t1, t2;
    struct tm tm1, tm2;
    const char *time1 = f_time1.c_str();
    const char *time2 = f_time2.c_str();

    strptime(time1, "%Y-%m-%d %H:%M", &tm1);
    strptime(time2, "%Y-%m-%d %H:%M", &tm2);

    t1 = mktime(&tm1);
    t2 = mktime(&tm2);

    return difftime(t1, t2) > 0.0;
}

bool X_compare(string name1, vector<string> params1, string name2, vector<string> params2) {
    return params1[0] > params2[0];
}

bool U_compare(string name1, vector<string> params1, string name2, vector<string> params2) {
    return false;
}


void  options_runner_files(Options &opts, vector<string> &ls_files) {
    map<string, vector<string>> sort_map;

    std::ostringstream oss;
    oss << compile_dir << "/" << cur_dir;
    string cur_dir_ = oss.str();

    for (auto &file : ls_files) {
        // map [name: exension, size, date]
        get_details(file, sort_map, cur_dir_);
    }

    if (opts.use_N)
        ls_files = bubble_sort(N_compare, sort_map, ls_files, opts);
    else if (opts.use_S)
        ls_files = bubble_sort(S_compare, sort_map, ls_files, opts);
    else if (opts.use_t)
        ls_files = bubble_sort(t_compare, sort_map, ls_files, opts);
    else if (opts.use_X)
        ls_files = bubble_sort(X_compare, sort_map, ls_files, opts);
    else if (opts.use_U)
        ls_files = bubble_sort(U_compare, sort_map, ls_files, opts);
    else {
        std::cerr << "Error while sorting files" << std::endl;
        exit(EXIT_FAILURE);
    }

    string glob_dir = cur_dir_ + "/";
    vector<string> spec_files;

    for (const auto &file : ls_files) {
        if (file == "")
            continue;
        if (file[0] == '.')
            continue;
        if (opts.use_s) {
            if (is_spec_file((glob_dir + file).c_str())) {
                spec_files.emplace_back(file);
                continue;
            }
        }

        if (opts.use_F) {
            struct stat buf;
            stat((glob_dir + file).c_str(), &buf);
            if (S_ISSOCK(buf.st_mode))
                std::cout << "=";
            else if (S_ISLNK(buf.st_mode))
                std::cout << "@";
            else if (S_ISFIFO(buf.st_mode))
                std::cout << "|";
            else if (buf.st_mode && S_IEXEC)
                std::cout << "*";
            else if (!S_ISDIR(buf.st_mode) && !S_ISREG(buf.st_mode))
                std::cout << "?";
        }

        if (opts.use_l && file != "") {
            auto info = sort_map[file];
            string f_extension = info[0], f_size = info[1], f_date = info[2];
            std::cout << file << "\t\t" << f_size << "\t\t" << f_date << std::endl;
        } else {
            std::cout << file << "\t";
        }
    }
    if (opts.use_s) {
        std::cout << std::endl;
        for (const auto &file:spec_files) {
            struct stat buf;
            stat((glob_dir + file).c_str(), &buf);
            if (S_ISSOCK(buf.st_mode))
                std::cout << "=";
            else if (S_ISLNK(buf.st_mode))
                std::cout << "@";
            else if (S_ISFIFO(buf.st_mode))
                std::cout << "|";
            else if (buf.st_mode & S_IEXEC)
                std::cout << "*";
            else if (!S_ISDIR(buf.st_mode) && !S_ISREG(buf.st_mode))
                std::cout << "?";

            if (opts.use_l) {
                auto info = sort_map[file];
                string f_extension = info[0], f_size = info[1], f_date = info[2];
                std::cout << file << "\t\t" << f_size << "\t\t" << f_date << std::endl;
            } else {
                std::cout << file << "\t";
            }
        }

    }
    std::cout << std::endl;
}

vector<string> one_folder_cont(string &dir_path) {
    vector<string> content;
    struct dirent **namelist;
    char *file;
    string path_and_file;

    int n = scandir((dir_path).c_str(), &namelist, nullptr, alphasort);
    if (n == -1)
        return content;
    while (--n > 1) {
        file = namelist[n]->d_name;
        path_and_file = dir_path + (string) "/" + file;
        content.emplace_back(file);
        free(namelist[n]);
    }
    free(namelist);
    return content;
}

void show_folder_files(string path, Options &opts) {
    vector<string> fold_files = one_folder_cont(path);

    std::ostringstream oss;
    oss << compile_dir << "/" << cur_dir;
    string cur_dir_ = oss.str();
    map<string, vector<string>> sort_map;

    for (auto &file : fold_files) {
        // map [name: exension, size, date]
        get_details(file, sort_map, cur_dir_);
    }

    if (opts.use_N)
        fold_files = bubble_sort(N_compare, sort_map, fold_files, opts);
    else if (opts.use_S)
        fold_files = bubble_sort(S_compare, sort_map, fold_files, opts);
    else if (opts.use_t)
        fold_files = bubble_sort(t_compare, sort_map, fold_files, opts);
    else if (opts.use_X)
        fold_files = bubble_sort(X_compare, sort_map, fold_files, opts);
    else if (opts.use_U)
        fold_files = bubble_sort(U_compare, sort_map, fold_files, opts);
    else {
        std::cerr << "Error while sorting files" << std::endl;
        exit(EXIT_FAILURE);
    }

//    if (opts.use_R)
    std::cout << path << ":" << std::endl;

    bool isdir;
    string glob_path = path + '/';
    vector<string> spec_files;

    for (const auto &file : fold_files) {
        if (file == "")
            continue;
        if (file[0] == '.')
            continue;
        if (opts.use_s) {
            if (is_spec_file((glob_path + file).c_str())) {
                spec_files.emplace_back(file);
                continue;
            }
        }
        isdir = is_dir((glob_path + file).c_str());
        if (isdir)
            std::cout << "/";
        else
            std::cout << "";

        if (opts.use_F) {
            struct stat buf;
            stat((glob_path + file).c_str(), &buf);
            if (S_ISSOCK(buf.st_mode))
                std::cout << "=";
            else if (S_ISLNK(buf.st_mode))
                std::cout << "@";
            else if (S_ISFIFO(buf.st_mode))
                std::cout << "|";
            else if (buf.st_mode && S_IEXEC)
                std::cout << "*";
            else if (!S_ISDIR(buf.st_mode) && !S_ISREG(buf.st_mode))
                std::cout << "?";
        }

        if (opts.use_l && file != "") {
            auto info = sort_map[file];
            string f_extension = info[0], f_size = info[1], f_date = info[2];
            std::cout << file << "\t\t" << f_size << "\t\t" << f_date << std::endl;
        } else {
            std::cout << file << "\t";
        }
    }
    if (opts.use_s) {
        std::cout << std::endl;
        for (const auto &file:spec_files) {
            struct stat buf;
            stat((glob_path + file).c_str(), &buf);
            if (S_ISSOCK(buf.st_mode))
                std::cout << "=";
            else if (S_ISLNK(buf.st_mode))
                std::cout << "@";
            else if (S_ISFIFO(buf.st_mode))
                std::cout << "|";
            else if (buf.st_mode & S_IEXEC)
                std::cout << "*";
            else if (!S_ISDIR(buf.st_mode) && !S_ISREG(buf.st_mode))
                std::cout << "?";

            if (opts.use_l) {
                auto info = sort_map[file];
                string f_extension = info[0], f_size = info[1], f_date = info[2];
                std::cout << file << "\t\t" << f_size << "\t\t" << f_date << std::endl;
            } else {
                std::cout << file << "\t";
            }
        }

    }
    std::cout << std::endl << std::endl;

    if (opts.use_R) {
        for (const auto &file : fold_files) {
            if (file[0] == '.') continue;
            std::ostringstream oss;
            oss << cur_dir_ << "/" << file;
            string dir = oss.str();

            if (is_dir(dir.c_str())) {
                vector<string> next_ls_files_dir;
                next_ls_files_dir.emplace_back(glob_path + file);
                show_folder_files(glob_path + file, opts);
            }
        }
    }
}


/*
 * Function clears up the vector of user command with no wildcards.
 *
 * return: vector usr_commands with the added wildcard filenames,
 *          deleted wildcards from the vector
 * */
void add_wildcards_to_command(vector<string> &usr_commands) {
    vector<string> tmp_get_filenames;
    std::map<int, vector<string>> pos_filenames;

    int find_wildcards = 0;
    size_t counter = 0;

    for (const auto &argum : usr_commands) {
        find_wildcards = wild_just_end(argum); // if valid wildcard

        // if valid
        if (find_wildcards == 1) {

            tmp_get_filenames = get_particular_wildcard(argum);
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
int wild_just_end(const string& argum) {
    vector<string> splitted_argum;

    boost::algorithm::split(splitted_argum, argum, is_any_of("/"));

    for (string path_part : splitted_argum) {
        std::cout << path_part << std::endl;
        if (path_part.find("*") != string::npos) {
            // if found
            if (splitted_argum.back() != path_part) {
                // at the middle of the path or in the program name
                return 0;
            } else if (splitted_argum.front() == path_part) {
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

void get_details(string &f_name, map<string, vector<string>> &map_all_files, string cur_dir) {
    struct stat file_info{};
    vector<string> details;
    if (cur_dir[cur_dir.size()-1] == '.') {
        cur_dir.pop_back();
    }
    // xxx
    if (stat( (cur_dir + (string) "/" + f_name).c_str(), &file_info) != 0) {
        // failed
    }

    // remove extension
    size_t found = f_name.find_last_of(".");
    if (found != std::string::npos)
        details.emplace_back(f_name.substr(found+1));
    else
        details.emplace_back("'folder'");

    // size
    string size;
    std::stringstream sstr;
    sstr << (unsigned long long) file_info.st_size;
    sstr >> size;
    details.emplace_back(size);

    // time last modified
    struct tm tm;
    char buf[224];
    localtime_r(&file_info.st_mtime, &tm);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H: %M", &tm);

    details.emplace_back(buf);
    map_all_files[f_name] = details;
}

bool is_dir(const char *path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

bool is_spec_file(const char*path) {
    struct stat buf;
    stat(path, &buf);
    if (S_ISSOCK(buf.st_mode))
        return true;
    else if (S_ISLNK(buf.st_mode))
        return true;
    else if (S_ISFIFO(buf.st_mode))
        return true;
    else if (buf.st_mode & (S_IEXEC != 0))
        return true;
    return false;
}

vector<string> bubble_sort(bool (*comparison)(string, vector<string>, string, vector<string>),
                           map<string, vector<string>> params, vector<string> args, Options &opts) {
    int n = args.size();
    string tmp;

    for (int i = 0; i < n-1; ++i) {
        for (int j = 0; j < n-i-1; ++j) {
            string frst = args[j];
            string sec = args[j+1];
            bool swap = comparison(frst, params[frst], sec, params[sec]);
            if (opts.use_D) {
                std::ostringstream oss1;
                oss1 << compile_dir << "/" << frst;
                string f_dir = oss1.str();

                std::ostringstream oss2;
                oss2 << compile_dir << "/" << sec;
                string s_dir = oss2.str();

                bool fd = is_dir(f_dir.c_str()), sd = is_dir(s_dir.c_str());

                if (fd && !sd)
                    swap = false;
                else if (!fd && sd)
                    swap = true;
            }
            if (opts.use_r)
                swap = !swap;

            if (swap) {
                tmp = frst;
                args[j] = sec;
                args[j+1] = tmp;
            }
        }
    }
    return args;
}