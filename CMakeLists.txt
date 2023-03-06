cmake_minimum_required(VERSION 3.24)
project(Loggerhead_Shell C)

set(CMAKE_C_STANDARD 17)

add_library(Loggerhead_Shell SHARED library.c loggerhead_shell.c)
