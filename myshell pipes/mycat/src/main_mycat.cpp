#include <iostream>
#include <boost/program_options.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "../inc/operations/io_mycat.h"

ssize_t buf_size = 256;

int main(int argc, char **argv) {
    std::vector<std::string> file_names;

    namespace po = boost::program_options;

    po::options_description visible("Supported options");
    visible.add_options()
            ("help,h", "Print a help message.")
            ("A", "All the invisible symbols (except whitespaces) are printed in hexadecimal codes.");

    std::string const FILES_KEY("files");
    po::options_description hidden("Hidden options");
    hidden.add_options()
            (FILES_KEY.c_str(), po::value(&file_names)->required(), "List of files.");

    po::positional_options_description p;
    p.add(FILES_KEY.c_str(), -1);

    po::options_description all("All options.");
    all.add(visible).add(hidden);

    po::variables_map vm;
    po::store(
            po::command_line_parser(argc, argv).
            options(all).
            positional(p).
            style(po::command_line_style::default_style | po::command_line_style::allow_long_disguise).
            run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage:\n  ./mycat [options] [-h|--help] [-A] <file1 file2... filen>\n" << visible << std::endl;
        return EXIT_SUCCESS;
    }

    bool flag_a = false;
    if (vm.count("A")) {
        flag_a = true;
    }

    int fd = 0, status = 0;
    ssize_t already_read = 0;

    std::vector<int> fd_vector;

    for (auto file : file_names) {
        fd = io::open_(file.c_str(), O_RDONLY);
        if (fd == -1) {
            const char* err_m = ("File doesn't exist." + file).c_str();
            write(STDERR_FILENO, err_m, strlen(err_m));
            return 1;
        }
        fd_vector.emplace_back(fd);
    }

    for (auto fd : fd_vector) {
        struct stat file_stat{};
        fstat(fd, &file_stat);
        auto f_size = file_stat.st_size;

        char* buf = (char *)malloc(f_size + 1);

        while (already_read != f_size) {
            if (f_size - already_read < buf_size) {
                buf_size = f_size - already_read;
            }
            if (io::readbuffer(fd, buf, buf_size, &status) < 0) {
                // if any error has happened
                const char *err_m = ("Read error status: " + std::to_string(status)).c_str();
                write(STDERR_FILENO, err_m, strlen(err_m));
                return -3;
            }

            if (io::writebuffer(STDOUT_FILENO, buf, buf_size, &status, &flag_a) < 0) {
                const char *err_m = ("Write error status: " + std::to_string(status)).c_str();
                write(STDERR_FILENO, err_m, strlen(err_m));
                return -4;
            }
            already_read += buf_size;
        }
        close(fd);
        already_read = 0;
    }

    return EXIT_SUCCESS;
}
