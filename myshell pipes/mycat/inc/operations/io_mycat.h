#ifndef IO_HPP
#define IO_HPP

#include <unistd.h>
#include <string>

namespace io {
    int open_(const char*, int);
    int writebuffer(int, const char*, ssize_t, int*, bool*);
    int readbuffer(int, char*, ssize_t, int*);
}

#endif //ADDER_OPERATIONS_HPP
