cmake_minimum_required(VERSION 3.23)
project(test_skiplist)

set(CMAKE_CXX_STANDARD 14)

add_executable(test_skiplist main.cpp
        SkipList.h
        )
