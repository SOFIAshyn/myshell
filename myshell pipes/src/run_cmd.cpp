#include <operations/run_cmd.h>
#include <operations/operations.hpp>
#include <operations/parse.h>
#include <boost/algorithm/string.hpp>

extern char **environ;
shellCommands shellCommands;


int run_line(vector<string> &line_usr_comands, std::map<string, int> &stream_, const bool& back_exec, int *read_from, int *write_to) {
    int status;
    auto cmd_name = line_usr_comands[0];

    // execute the command
    myshellFunction myF;
    if ((myF = shellCommands.get_function(cmd_name)) != nullptr) {
        bool h_flag = false;
        for (auto arg : line_usr_comands) {
            if (arg == "-h" || arg == "--help")
                h_flag = true;
        }
        status = myF(line_usr_comands, h_flag);
        status_merrno = status;
        return status;
    }

    pid_t pid = fork(); // create new process, by duplicating the calling process.

    if (pid == -1) {
        std::cerr << "Failed fork()" << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // check if its a background command
        if (back_exec == false) {
            waitpid(pid, &status, 0); // wait the process to change state
            if (status == EXIT_FAILURE)
                std::cout << "Error while executing background command.";
            status_merrno = status;
        }
    } else if (pid == 0) { // child reads from pipe
        if (read_from != nullptr && write_to != nullptr) {
            //doesn't read from the pipe to which we write
            close(write_to[0]);
            // doesnt write to the pipe from which we read
            close(read_from[1]);
            // write to the pipe into which we wanna write
            dup2(write_to[1], STDOUT_FILENO);
            // read from the pipe from which we wanna read
            dup2(read_from[0], STDIN_FILENO);
        } else {
            if (write_to != nullptr) { // write to the next pipe
                //doesn't read from the pipe to which we write
                close(write_to[0]);
                // write to the pipe into which we wanna write
                dup2(write_to[1], STDOUT_FILENO);
            }
            if (read_from != nullptr) { // read from prev pipe
                // doesnt write to the pipe from which we read
                close(read_from[1]);
                // read from the pipe from which we wanna read
                dup2(read_from[0], STDIN_FILENO);
            }
        }

        if (boost::filesystem::extension(cmd_name) == ".msh") {
            line_usr_comands.insert(line_usr_comands.begin(), ".");
            status = shellCommands.get_function(".")(line_usr_comands, false);
            status_merrno = status;
            exit(status);
        }
        int how_to_open;
        for (const auto & redirect:stream_) {
            if (redirect.second == STDIN_FILENO)
                how_to_open = O_RDONLY;
            else
                how_to_open = O_WRONLY;
            auto fd = open(redirect.first.c_str(), how_to_open);
            if (fd == -1) {
                if (redirect.second == STDOUT_FILENO && redirect.first == STDERR_NUM) {
                    dup2(STDOUT_FILENO, STDERR_FILENO);
                    continue;
                } else {
                    std::cout << "Error while openning file for the redirection." << std::endl;;
                    return fd;
                }
            }
            dup2(fd, redirect.second);
        }

        // create new array, reserve the place for it
        vector<const char *> cmd_args;
        cmd_args.reserve(line_usr_comands.size() + 1);

        // go through all the user commands
        for (const auto &arg : line_usr_comands) {
            cmd_args.emplace_back(arg.c_str());
        }
        cmd_args.emplace_back(nullptr);

        // returns changed type and can be used to cast away (remove) CONSTness
        auto exec_cmd_args = const_cast<char* const*>(cmd_args.data());
        execvp(cmd_name.c_str(), exec_cmd_args); // replaces the current process image with a new process image

        std::cerr << cmd_name << ": not found " << std::endl;

        exit(errno);
    }
    return status;
}
