cmake_minimum_required(VERSION 3.10)

project(MyCppProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Enable generation of 'compile_commands.json' file for 'clangd' autocompletion and referencing: Works only with Unix Makefiles generator on Linux platform and Ninja generator for multiple platforms Win11/Linux/macOS
# TODO add message that with Visual Studio generator the generating the compile_commands.json file isn't supported: only Unix Makefiles or Ninja support generating compile_commands.json file for clangd intellisense

add_executable(my_cpp_project
    src/main.cpp
)

set(VSCODE_MICROSOFT_CPP_EXTENSION_PLATFORM_SPECIFIC_INTELLISENSE_STATE 
    "\"C_Cpp.intelliSenseEngine\": \"disabled\"") # Disable IntelliSense from Microsoft C/C++ extension in favor of clangd, but only on non-Windows systems

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(VSCODE_MICROSOFT_CPP_EXTENSION_PLATFORM_SPECIFIC_INTELLISENSE_STATE
        "\"C_Cpp.intelliSenseEngine\": \"default\", // \"default\" engine has more complete syntax completion, but reports false positive ill-formed code by red-squiggly underline and has higher latency
    //\"C_Cpp.intelliSenseEngine\": \"Tag Parser\", // \"Tag Parser\" engine has less complete syntax completion (e.g. ranges), but reports no false positives and has lower latency
    \"C_Cpp.default.cppStandard\": \"c++${CMAKE_CXX_STANDARD}\""
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

# TODO generate '.vscode/launch.json' for VSCode debugger extension
#   Linux: GDB, clang (LLVM/LLDB)
#   Windows: MSVC, clang (LLVM/LLDB), clang (LLVM/LLDB - MSVC compatible API), MinGW
#   macOS: clang (LLVM/LLDB), Apple clang (LLVM/LLDB - Apple), gdb

message("Platform name: ${CMAKE_SYSTEM_NAME}")
message("Compiler name: ${CMAKE_CXX_COMPILER_ID}")
message("Build type=" ${CMAKE_BUILD_TYPE}) # TODO make more robust for multi-configuration makefile generators like 'MS Visual Studio' generator
message("Build directory: ${CMAKE_BINARY_DIR}")
message("Source directory: ${CMAKE_SOURCE_DIR}")
message("Current binary directory: ${CMAKE_CURRENT_BINARY_DIR}")
get_target_property(OUTPUT_NAME my_cpp_project OUTPUT_NAME)
message("Explicit name of binary: ${OUTPUT_NAME}")
get_target_property(TARGET_NAME my_cpp_project NAME)
message("Implicit name of binary: ${TARGET_NAME}")

set(BINARY_ABSOLUTE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}")

set(LAUNCH_JSON_DEBUG_CONFIGURATIONS)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message("Generate 'launch.json' debug configuration for VSCode compatible with MSVC for Windows")
    set(DEBUG_SESSION_NAME "Start debugging session on ${CMAKE_SYSTEM_NAME} for '${TARGET_NAME}' compiled by ${CMAKE_CXX_COMPILER_ID}")
    set(TARGET_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/Debug/${TARGET_NAME}.exe")
    set(LAUNCH_JSON_DEBUG_CONFIGURATIONS [=[
"type": "cppvsdbg", // DIFFERENT

            "request": "launch", // SAME

            //"program": "${workspaceFolder}/build/Debug/my_cpp_project.exe", // DIFFERENT // Location of the compiled executable formatted as forward-slashed-Windows-style path
            //"program": "${command:cmake.launchTargetPath}", // DIFFERENT // Resolves target executable path by 'CMake Tools' VSCode extension: slower debug session startup, more magical, but with better cross-compatibility

            "args": [], // SAME // enter the arguments for the program when needed: single letter arguments '-o file.txt' or '/o file.txt', keyword arguments '--file input_file.txt' or an arbitrary text 'someArgument' or '"multiple words as one argument"' in quotes
            "stopAtEntry": false, // SAME
            "cwd": "${workspaceFolder}", // SAME
            "environment": [], // SAME

            "console": "internalConsole" // UNIQUE MSVC
        }]=])
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message("Generate 'launch.json' debug configuration for VSCode compatible with GCC (possibly Linux, might be macOS or a different platform)")
    set(DEBUG_SESSION_NAME "Start debugging session on ${CMAKE_SYSTEM_NAME} for '${TARGET_NAME}' compiled by ${CMAKE_CXX_COMPILER_ID}, // DIFFERENT")
    set(TARGET_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}")
    set(LAUNCH_JSON_DEBUG_CONFIGURATIONS [=[
"type": "cppdbg", // DIFFERENT

            "request": "launch", // SAME

            //"program": "${workspaceFolder}/build-Debug/my_cpp_project",  // DIFFERENT // Location of the compiled executable formatted as UNIX-style path
            //"program": "${command:cmake.launchTargetPath}", // DIFFERENT // Resolves target executable path by 'CMake Tools' VSCode extension: slower debug session startup, more magical, but with better cross-compatibility

            "args": [],  // SAME // enter the arguments for the program when needed: single letter arguments '-o file.txt', keyword arguments '--file input_file.txt' or an arbitrary text 'someArgument' or '"multiple words as one argument"' in quotes
            "stopAtEntry": false, // SAME
            "cwd": "${workspaceFolder}", // SAME
            "environment": [],  // SAME // modify PATH variable; add custom system variables

            "externalConsole": false, // UNIQUE GCC
            "MIMode": "gdb", // UNIQUE GCC
            "miDebuggerPath": "/usr/bin/gdb", // UNIQUE GCC
            "setupCommands": [ // UNIQUE GCC
                { // UNIQUE GCC
                    "description": "Enable pretty-printing for gdb", // UNIQUE GCC
                    "text": "-enable-pretty-printing", // UNIQUE GCC
                    "ignoreFailures": true // UNIQUE GCC
                } // UNIQUE GCC
            ] // UNIQUE GCC
        }]=])
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message("Generate 'launch.json' debug configuration for VSCode compatible with Clang or AppleClang (possibly macOS, might be Linux, Windows or a different platform)")
    set(LAUNCH_JSON_DEBUG_CONFIGURATIONS [=[{
            "version": "0.2.0",
            "configurations": [
                {
                    "name": "C/C++: LLDB Debug", // DIFFERENT
                    "type": "cppdbg", // DIFFERENT

                    "request": "launch",

                    "program": "${workspaceFolder}/build/my_project", // DIFFERENT

                    "args": [],
                    "stopAtEntry": false,
                    "cwd": "${workspaceFolder}",
                    "environment": [],

                    "externalConsole": false, // UNIQUE CLANG
                    "MIMode": "lldb" // UNIQUE CLANG
                }
            ]
        }]=])
endif()

configure_file(launch.json.in ${CMAKE_SOURCE_DIR}/.vscode/launch.json @ONLY)

add_custom_command(TARGET my_cpp_project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Binary path: $<TARGET_FILE:my_cpp_project>"
)

# TODO generate '.clangd', but only if on Linux
# TODO add CMakePresets.json for multiplatform compatibility
# TODO integrate CPM dependency manager + integrate Google Test via CPM
# TODO test on macOS and FreeBSD
