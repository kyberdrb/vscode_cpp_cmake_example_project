cmake_minimum_required(VERSION 3.10)

project(MyCppProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Enable generation of 'compile_commands.json' file for 'clangd' autocompletion and referencing: Works only with Unix Makefiles generator on Linux platform and Ninja generator for multiple platforms Win11/Linux/macOS

message("CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})

add_executable(my_cpp_project
    src/main.cpp
)

set(VSCODE_MICROSOFT_CPP_EXTENSION_PLATFORM_SPECIFIC_INTELLISENSE_STATE 
    "\"C_Cpp.intelliSenseEngine\": \"disabled\"") # Disable IntelliSense from Microsoft C/C++ extension in favor of clangd, but only on non-Windows systems

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(VSCODE_MICROSOFT_CPP_EXTENSION_PLATFORM_SPECIFIC_INTELLISENSE_STATE
        "\"C_Cpp.intelliSenseEngine\": \"default\", // \"default\" engine has more complete syntax completion, but reports false positive ill-formed code by red-squiggly underline and has higher latency
        //\"C_Cpp.intelliSenseEngine\": \"Tag Parser\", // \"Tag Parser\" engine has less complete syntax completion (e.g. ranges), but reports no false positives and has lower latency
        \"C_Cpp.default.cppStandard\": \"c++23\""
    )
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)
    set(CLANGD_CONFIGURATION
        ",\"clangd.arguments\": [
        \"--compile-commands-dir=${BUILD_DIR_NAME}\" // Tell clangd to use compile_commands.json from build directory
    ]"
)
endif()

configure_file(settings.json.in ${CMAKE_SOURCE_DIR}/.vscode/settings.json @ONLY)

# TODO add CMakePresets.json for multiplatform compatibility
# TODO generate '.vscode/launch.json' for VSCode debugger extension
# TODO generate '.clangd', but only if on Linux
# TODO integrate CPM dependency manager + integrate Google Test via CPM
