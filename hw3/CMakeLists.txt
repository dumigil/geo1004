cmake_minimum_required(VERSION 3.17)
project(TIN)
set(CMAKE_CXX_STANDARD 17)

add_definitions(
        -DJSON_ELEV_PATH=\"${PROJECT_SOURCE_DIR}/3d_one.json\"
)
add_executable(TIN
        main.cpp)
