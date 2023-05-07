cmake_minimum_required(VERSION 3.25)
project(executor2)

set(CMAKE_CXX_STANDARD 20)

add_executable(executor2 main.cpp task_manager.cpp input_parser.cpp task.cpp utils.cpp)
