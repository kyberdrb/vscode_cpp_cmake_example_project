cmake_minimum_required(VERSION 3.10)

project(MyCppProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Enable generation of 'compile_commands.json' file for clangd autocompletion and referencing

message("CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})

add_executable(my_cpp_project
    src/main.cpp
    src/FluentBuilder.h
    src/FluentBuilder.cpp
)
