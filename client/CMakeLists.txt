cmake_minimum_required(VERSION 3.30)
project(client C)

set(CMAKE_C_STANDARD 11)

add_executable(client main.c
        httpImp.c
        httpImp.h
        json-tools.c
        json-tools.h
        global-variables.h
        global-variables.c)

target_link_libraries(client json-c)
