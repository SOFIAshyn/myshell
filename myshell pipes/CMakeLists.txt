cmake_minimum_required(VERSION 3.8)
project(myshell CXX)

set(CMAKE_CXX_STANDARD 17)

# Options
#option(WARNINGS_AS_ERRORS   "Treat compiler warnings as errors."    OFF)
#option(ENABLE_SANITIZERS    "Use sanitizers to detect errors."      OFF)
#option(ENABLE_CONAN         "Use Conan as a package manager."       OFF)
#
# Include fixed CMake configuration
#include(cmake/config.cmake)

# Project source compilation
include_directories(inc mycat myls)
add_library(operations SHARED
        src/operations.cpp
        src/parse.cpp
        src/run_cmd.cpp
        src/commands/merrno.cpp
        src/commands/dot_cmd.cpp
        src/commands/mpwd.cpp
        src/commands/mexit.cpp
        src/commands/mecho.cpp
        src/commands/mexport.cpp
        src/commands/mcd.cpp
        inc/operations/operations.hpp
        inc/operations/main.h
        inc/operations/parse.h
        inc/operations/run_cmd.h
        )
add_executable(myshell src/main.cpp)
target_link_libraries(myshell operations)
add_subdirectory(myls)
add_subdirectory(mycat)

# Add external packages
find_package(Boost 1.65.1 COMPONENTS program_options system filesystem REQUIRED)

find_library(_READLINE_LIB readline)

if (NOT _READLINE_LIB)
    message(FATAL_ERROR "Not found libreadline.a")
endif ()
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(myshell readline ${Boost_LIBRARIES})

# Sanitizers configuration
#if (ENABLE_SANITIZERS)
#    set(SANITIZE_UNDEFINED ON)
    # Only one of Memory, Address, or Thread sanitizers is applicable at the time
#    set(SANITIZE_MEMORY ON)
#    set(SANITIZE_ADDRESS ON)
#    set(SANITIZE_THREAD ON)
#    find_package(Sanitizers)

    # CMake targets to which apply sanitizers
#    add_sanitizers(operations myshell)
#endif ()
