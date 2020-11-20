# Myshell Pipess

## Team

 - [Petryshyn Sofiia](https://github.com/SOFIAshyn)

## Prerequisites

 - **C++ compiler** - needs to support **C++17** standard
 - **CMake** 3.8+
 
## Installing

1. Clone the project.
    ```bash
    git clone git@github.com/SOFIAshyn/myshell.git
    ```
2. Install required libraries. On Ubuntu:
   ```bash
   sed 's/\r$//' apt_packages.txt | sed 's/#.*//' | xargs sudo apt-get install -y
   ```
3. Build.
    ```bash
    mkdir build
    cd build
    cmake ../
    make
    ./myshell
    ```
## Usage
Run your commads in myshell.
   ```bash
   # simple command
   ls
   
   # redirection
   ls > t.txt
   
   # data redirection
   ls | grep *.txt
   
   # background process
   ls &
   
   # declare your env variable
   VAR=$(echo "my var")
   
   # show the env variable
   $VAR
   
   # use myls
   myls
   
   # use mycat
   mycat t.txt
   ```

Help flags `-h`/`--help` support is available.
