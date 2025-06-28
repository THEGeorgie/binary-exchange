cmake_minimum_required(VERSION 3.30)
project(server C)

set(CMAKE_C_STANDARD 11)

add_executable(server main.c
        httpImp.c
        httpImp.h
        json-tools.c
        json-tools.h
        usrProcMang.c
        usrProcMang.h)

target_link_libraries(server json-c)

