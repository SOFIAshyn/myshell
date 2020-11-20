cmake_minimum_required(VERSION 3.8)
project(mycat CXX)

set(CMAKE_CXX_STANDARD 17)

# Project source compilation
add_executable(mycat src/main_mycat.cpp src/io_mycat.cpp inc/operations/io_mycat.h)
set(EXECUTABLE_OUTPUT_PATH ../external_cmds)

# Add external packages
find_package(Boost 1.65.1 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(mycat Boost::program_options)