cmake_minimum_required(VERSION 3.18)
project(executor2)

set(CMAKE_CXX_STANDARD 20)

add_executable(executor main.cpp task_manager.cpp input_parser.cpp task.cpp)

