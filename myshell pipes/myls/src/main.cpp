#include "../inc/operations/main_myls.h"
#include "../inc/operations/io_myls.h"


int main(int argc, char **argv) {
    string files;
    vector<string> ls_files;
    vector<string> ls_dirs;

    auto opts = get_ops(argc, argv, files);
    boost::split(ls_files, files, boost::is_any_of(" "));
    add_wildcards_to_command(ls_files);

    check_files_on_existance(ls_files, ls_dirs);
    options_runner_files(opts, ls_files);

    for (const auto &fold:ls_dirs) {
        show_folder_files(fold, opts);
    }

    return 0;
}