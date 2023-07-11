cmake_minimum_required(VERSION 3.25)
project(rtes_timer)
set(CMAKE_CXX_STANDARD 20)

add_executable(rtes_timer main.cpp)
add_compile_options(-pthread)

