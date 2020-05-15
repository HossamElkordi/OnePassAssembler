cmake_minimum_required(VERSION 3.16)
project(onepassassembler)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Assembler)

add_executable(onepassassembler
    Assembler/src/assembler.cpp)
