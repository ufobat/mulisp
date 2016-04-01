cmake_minimum_required(VERSION 3.3)
project(muforth)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCE_FILES main.c lexer.c)
add_executable(muforth ${SOURCE_FILES})