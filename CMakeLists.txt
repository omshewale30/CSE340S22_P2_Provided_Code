cmake_minimum_required(VERSION 3.23)
project(CSE340S22_P2_Provided_Code)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(CSE340S22_P2_Provided_Code
        inputbuf.cc
        inputbuf.h
        lexer.cc
        lexer.h
        project2.cc)
