cmake_minimum_required(VERSION 4.1)
project(Tetris)

set(CMAKE_CXX_STANDARD 20)

add_executable(Tetris main.cpp)


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
