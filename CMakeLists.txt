cmake_minimum_required(VERSION 3.13)
project(File_System C)

set(CMAKE_C_STANDARD 99)

add_executable(File_System main.c diskio.c diskio.h fileio.c fileio.h)