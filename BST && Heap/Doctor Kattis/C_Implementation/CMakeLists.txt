cmake_minimum_required(VERSION 4.0)
project(C_Implementation)

set(CMAKE_CXX_STANDARD 20)

add_executable(C_Implementation main.cpp
        rbtree.cpp
        rbtree.h
        hash_set.cpp
        hash_set.h)
