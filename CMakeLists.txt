cmake_minimum_required(VERSION 3.3)
project(mulisp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCE_FILES main.c tokenize.c list.c misc.c parser.c)
add_executable(mulisp ${SOURCE_FILES})