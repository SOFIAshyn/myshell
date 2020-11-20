//
// Created by olyapubuntu on 18.10.20.
//

#ifndef MYSHELL_RUN_CMD_H
#define MYSHELL_RUN_CMD_H
#include "operations/main.h"

extern int __thread status_merrno;

int run_line(vector<string> &line_usr_comands, std::map<string, int> &stream_, const bool& back_exec, int *read_from, int *write_to);

#endif //MYSHELL_RUN_CMD_H
