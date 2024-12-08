cmake_minimum_required(VERSION 3.10)

# Project name and version
project(MyCppProject VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the source file to compile
add_executable(my_cpp_project src/main.cpp)

# Enable compile_commands.json generation for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
