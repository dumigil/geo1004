cmake_minimum_required(VERSION 3.17)
project(voxeliser)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(voxeliser
        main.cpp
        Point.h
        Rows.h
        VoxelGrid.h)
