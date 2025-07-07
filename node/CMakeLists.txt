cmake_minimum_required(VERSION 3.31)
project(node C)

set(CMAKE_C_STANDARD 11)

add_executable(node main.c
        httpImp.c
        httpImp.h
        json-tools.c
        json-tools.h)

target_link_libraries(node json-c)
