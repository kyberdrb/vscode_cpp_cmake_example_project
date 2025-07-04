cmake_minimum_required(VERSION 3.10)

project(MyCppProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Enable generation of 'compile_commands.json' file for 'clangd' autocompletion and referencing: Works only with Unix Makefiles generator on Linux platform and Ninja generator for multiple platforms Win11/Linux/macOS
# TODO add message that with Visual Studio generator the generating the compile_commands.json file isn't supported: only Unix Makefiles or Ninja support generating compile_commands.json file for clangd intellisense

set(EXECUTABLE_NAME "my_cpp_project")
add_executable(${EXECUTABLE_NAME}
    src/main.cpp
)

message("Generate 'settings.json' debug configuration for VSCode compatible with ${CMAKE_SYSTEM_NAME} platform")

set(VSCODE_MICROSOFT_CPP_EXTENSION_PLATFORM_SPECIFIC_INTELLISENSE_STATE 
    "\"C_Cpp.intelliSenseEngine\": \"disabled\"") # Disable IntelliSense from Microsoft C/C++ extension in favor of clangd, but only on non-Windows systems

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(VSCODE_MICROSOFT_CPP_EXTENSION_PLATFORM_SPECIFIC_INTELLISENSE_STATE
        "\"C_Cpp.intelliSenseEngine\": \"default\", // \"default\" engine has more complete syntax completion, but reports false positive ill-formed code by red-squiggly underline and has higher latency
    //\"C_Cpp.intelliSenseEngine\": \"Tag Parser\", // \"Tag Parser\" engine has less complete syntax completion (e.g. ranges), but reports no false positives and has lower latency
    \"C_Cpp.default.cppStandard\": \"c++${CMAKE_CXX_STANDARD}\" // Make the Microsoft's IntelliSense more accurate to find features of specified C++ standard in the standard library for the MSVC compiler"
    )
endif()

set(VSCODE_ENVIRONMENT_PLATFORM_AND_COMPILER_SPECIFIC_CONFIGURATIONS
    ${VSCODE_MICROSOFT_CPP_EXTENSION_PLATFORM_SPECIFIC_INTELLISENSE_STATE})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)
    set(CLANGD_CONFIGURATION
        ",
    \"clangd.arguments\": [
        \"--compile-commands-dir=${BUILD_DIR_NAME}\" // Tell clangd to use compile_commands.json from build directory
    ]")

    string( APPEND
        VSCODE_ENVIRONMENT_PLATFORM_AND_COMPILER_SPECIFIC_CONFIGURATIONS
        ${CLANGD_CONFIGURATION}
)
endif()

configure_file(settings.json.in ${CMAKE_SOURCE_DIR}/.vscode/settings.json @ONLY)

message("Generate 'launch.json' with debug configuration for VSCode, compatible with '${CMAKE_SYSTEM_NAME}' platform and '${CMAKE_CXX_COMPILER_ID}' compiler (assuming that this is the default toolchain for the platform)")
set(DEBUG_SESSION_NAME "Start debugging session on ${CMAKE_SYSTEM_NAME} for '${EXECUTABLE_NAME}' compiled by ${CMAKE_CXX_COMPILER_ID}")
set(VSCODE_DEBUGGER_TYPE "cppdbg")
set(TARGET_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(VSCODE_DEBUGGER_TYPE "cppvsdbg")
    set(TARGET_BINARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/Debug/${EXECUTABLE_NAME}.exe")
    set(VSCODE_PLATFORM_AND_COMPILER_SPECIFIC_DEBUG_CONFIGURATIONS [=[
"console": "internalConsole"]=])

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    find_program(GDB_PATH gdb REQUIRED)
    set(VSCODE_PLATFORM_AND_COMPILER_SPECIFIC_DEBUG_CONFIGURATIONS [=[
"externalConsole": false,
            "MIMode": "gdb",
]=])
    string(APPEND VSCODE_PLATFORM_AND_COMPILER_SPECIFIC_DEBUG_CONFIGURATIONS
"            \"miDebuggerPath\": \"${GDB_PATH}\",\n")
    string(APPEND VSCODE_PLATFORM_AND_COMPILER_SPECIFIC_DEBUG_CONFIGURATIONS [=[
            "setupCommands": [
                {
                    "description": "Enable pretty-printing of variables and containers for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]]=])

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(VSCODE_PLATFORM_AND_COMPILER_SPECIFIC_DEBUG_CONFIGURATIONS [=[
"externalConsole": false,
            "MIMode": "lldb"]=])

else()
    message(ERROR "Unsupported compiler-platform combination: can't generate 'launch.json' debug configuration with ${CMAKE_CXX_COMPILER_ID} compiler on ${CMAKE_SYSTEM_NAME}. Not a platform's default toolchain?")
    return()
endif()

configure_file(launch.json.in ${CMAKE_SOURCE_DIR}/.vscode/launch.json @ONLY)

# TODO generate '.clangd', but only if on Linux & GNU/GCC
# TODO test on macOS and FreeBSD
