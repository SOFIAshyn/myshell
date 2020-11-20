//
// Created by olyapubuntu on 18.10.20.
//

#ifndef MYSHELL_MAIN_H
#define MYSHELL_MAIN_H
#include <iostream>
#include <unordered_map>
#include <vector>
#include <map>

const int DIR_NAME_LEN = 2000;

using std::vector;
using std::map;
using std::string;

// define vector of strings - as myshell function
typedef int (*myshellFunction)(const vector<string> &, bool);

// store myshell command as an executable that we have to execute
// after execution we return output to the user
typedef std::unordered_map<string, myshellFunction> commands_map;

string set_path();

#endif //MYSHELL_MAIN_H
