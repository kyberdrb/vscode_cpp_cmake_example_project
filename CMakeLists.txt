cmake_minimum_required(VERSION 3.10)

project(MyCppProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Enable generation of 'compile_commands.json' file for 'clangd' autocompletion and referencing: Works only with Unix Makefiles generator on Linux platform and Ninja generator for multiple platforms Win11/Linux/macOS
# TODO add message that with Visual Studio generator the generating the compile_commands.json file isn't supported: only Unix Makefiles or Ninja support generating compile_commands.json file for clangd intellisense

add_executable(my_cpp_project
    src/main.cpp
)

message("Generate 'settings.json' debug configuration for VSCode compatible with ${CMAKE_SYSTEM_NAME} platform")
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

message("Generate 'launch.json' debug configuration for VSCode compatible with ${CMAKE_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}")
set(DEBUG_SESSION_NAME "Start debugging session on ${CMAKE_SYSTEM_NAME} for '${TARGET_NAME}' compiled by ${CMAKE_CXX_COMPILER_ID}")
set(VSCODE_DEBUGGER_TYPE "cppdbg")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(VSCODE_DEBUGGER_TYPE "cppvsdbg")
    set(TARGET_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/Debug/${TARGET_NAME}.exe")
    set(VSCODE_UNIQUE_DEBUG_CONFIGURATIONS [=[
"console": "internalConsole"]=])
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(TARGET_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}")

    find_program(GDB_PATH gdb REQUIRED)
    message("Found GDB at: ${GDB_PATH}")

    set(VSCODE_UNIQUE_DEBUG_CONFIGURATIONS
"\"externalConsole\": false,
            \"MIMode\": \"gdb\",
            \"miDebuggerPath\": \"${GDB_PATH}\",
            \"setupCommands\": [
                {
                    \"description\": \"Enable pretty-printing of variables and containers for gdb\",
                    \"text\": \"-enable-pretty-printing\",
                    \"ignoreFailures\": true
                }
            ]")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(VSCODE_UNIQUE_DEBUG_CONFIGURATIONS [=[
"externalConsole": false,
            "MIMode": "lldb"]=])
endif()

configure_file(launch.json.in ${CMAKE_SOURCE_DIR}/.vscode/launch.json @ONLY)

# TODO generate '.clangd', but only if on Linux
# TODO add CMakePresets.json for multiplatform compatibility
# TODO integrate CPM dependency manager + integrate Google Test via CPM
# TODO test on macOS and FreeBSD
