cmake_minimum_required(VERSION 4.2)
project(GridSNORT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

add_executable(GridSNORT main.cpp
        snort.h
        perft.h
        solver.h
        tt.h)
