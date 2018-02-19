cmake_minimum_required(VERSION 3.9)
project(assembler C)

set(CMAKE_C_STANDARD 90)

add_executable(assembler main.c common.h utils.c symbol_table.c first_scan.c second_scan.c symbol_table.h utils.h data_block.c data_block.h)