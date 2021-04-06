cmake_minimum_required(VERSION 3.17)
project(trees)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(trees
        Points.h
        trees.cpp)
