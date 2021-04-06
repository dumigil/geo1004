cmake_minimum_required(VERSION 3.17)
project(trees)

set(CMAKE_CXX_STANDARD 14)
find_package(CGAL REQUIRED)
include(${CGAL_USE_FILE})
include_directories(.)

add_executable(trees
        Points.h
        trees.cpp)
target_link_libraries(trees ${CGAL_LIBS})