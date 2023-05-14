cmake_minimum_required(VERSION 3.23)
project(kurs)

set(CMAKE_CXX_STANDARD 17)

add_executable(exact_solve helper.h exact_solver.cpp)
add_executable(euristic_solve helper.h euristic_solver.cpp)
add_executable(generate_tests helper.h generate_tests.cpp)
