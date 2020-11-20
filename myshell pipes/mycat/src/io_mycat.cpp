#include <sys/stat.h> // Structures and types for stat(), fstat(), lstat()
#include <fcntl.h> // Types and constants for fcntl(), open()
#include <unistd.h> // POSIX header
#include <iostream>
#include  "../inc/operations/io_mycat.h"
#include <sstream>


int io::open_(const char* filename, int oflag) {
    // int open(const char *pathname, int flags );
    int fd = open((const char *)filename, oflag);
    if (fd == ENOENT) {
        return -1;
    }
    return fd;
}

int io::readbuffer(int fd, char* buffer, ssize_t size, int* status) {
    ssize_t read_bytes = 0;

    // signal can interrupt the call - could be EINTR
    while(read_bytes < size) {
        // Ex: ssize_t read(int fd, void ∗buf, size_t count);
        ssize_t read_now = read(fd, buffer + read_bytes, size - read_bytes);
        if (read_now == 0) { // file has ended up
            break;
        } else if (read_now == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                *status = errno;
                return -1;
            }
        } else {
            read_bytes += read_now;
        }
    }

    return 0;
}

std::string hex_converter(const char* buffer, ssize_t size) {
    std::stringstream ss;
    for(int i = 0; i < size; ++i) {
        if (!isprint(buffer[i]) && !isspace(buffer[i]))
            ss << "\\x" << std::uppercase << std::hex << (unsigned int)(unsigned char)buffer[i];
        else
            ss << buffer[i];
    }
    return ss.str();
}


int io::writebuffer(int fd, const char* buffer, ssize_t size, int* status, bool* flag_a) {
    ssize_t written_bytes = 0;
    std::string str;

    if (*flag_a)
        str = hex_converter(buffer, size);

    ssize_t written_now;
    while(written_bytes < size) {
        if (*flag_a) {
            written_now = write(fd, str.c_str() + written_bytes, str.size() - written_bytes);
        } else {
            // Ex: ssize_t write (int fd, const void *buf, size_t count);
            written_now = write(fd, buffer + written_bytes, size - written_bytes);
        }
        if(written_now == -1) {
            if (errno == EINTR) { // avoid signals interrupt
                continue;
            } else{
                *status = errno;
                return -1;
            }
        } else {
            written_bytes += written_now;
        }
    }

    return 0;
}

