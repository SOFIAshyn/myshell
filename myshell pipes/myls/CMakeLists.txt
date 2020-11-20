cmake_minimum_required(VERSION 3.8)
project(myls CXX)

set(CMAKE_CXX_STANDARD 17)

# Project source compilation
add_executable(myls src/main.cpp src/io_myls.cpp inc/operations/main_myls.h inc/operations/io_myls.h)
set(EXECUTABLE_OUTPUT_PATH ../external_cmds)

# Add external packages
find_package(Boost 1.65.1 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(myls Boost::program_options)