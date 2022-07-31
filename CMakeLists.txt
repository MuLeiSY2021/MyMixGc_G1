cmake_minimum_required(VERSION 3.22)
project(MyGc C)

set(CMAKE_C_STANDARD 99)
add_executable(MyGc main.c virtualMemory.h tool.h MyMalloc.h)
