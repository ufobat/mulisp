cmake_minimum_required(VERSION 3.3)
project(mulisp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCE_FILES src/main.c src/tokenize.c src/misc.c src/parser.c src/env.c src/write.c src/eval.c src/init.c src/test.c)
add_executable(mulisp ${SOURCE_FILES})
