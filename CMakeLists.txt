cmake_minimum_required(VERSION 3.23)
project(kurs)

set(CMAKE_CXX_STANDARD 17)

add_executable(generate_tests helper.h generate_tests.cpp)

add_executable(exact_solve_task1 helper.h exact_solver_task1.cpp)
add_executable(euristic_solve_task1 helper.h euristic_solver_task1.cpp)

add_executable(exact_solve_task2 helper.h exact_solver_task2.cpp)
add_executable(solve_task2 helper.h solver_task2.cpp)

add_executable(solve_task3 helper.h solver_task3.cpp)