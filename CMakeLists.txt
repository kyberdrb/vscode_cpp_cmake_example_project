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

message("Generate 'settings.json' debug configuration for VSCode compatible with default toolchaing for ${CMAKE_SYSTEM_NAME} platform")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message("Setting up 'Microsoft C/C++' IntelliSense code completion and referencing")

    string(APPEND VSCODE_ENVIRONMENT_PLATFORM_AND_COMPILER_SPECIFIC_CONFIGURATIONS [=[
"C_Cpp.intelliSenseEngine": "default", // "default" engine is the updated engine, has more complete syntax completion, but reports false positive ill-formed code by red-squiggly underline and has slightly higher latency
    //"C_Cpp.intelliSenseEngine": "Tag Parser", // "Tag Parser" engine is the legacy engine, has less complete syntax completion (e.g. for 'ranges'), but reports no false positives and has lower latency
]=])
    string(APPEND VSCODE_ENVIRONMENT_PLATFORM_AND_COMPILER_SPECIFIC_CONFIGURATIONS
"    \"C_Cpp.default.cppStandard\": \"c++${CMAKE_CXX_STANDARD}\" // Make the Microsoft's IntelliSense more accurate to find features of specified C++ standard in the standard library for the MSVC compiler"
    )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU") # TODO what 'settings.json' do macOS & BSD need? add conditions and test on these platforms
    message("Setting up 'clangd' code completion and referencing")

    string(APPEND VSCODE_ENVIRONMENT_PLATFORM_AND_COMPILER_SPECIFIC_CONFIGURATIONS [=[
"C_Cpp.intelliSenseEngine": "disabled", // Disable IntelliSense from Microsoft C/C++ extension in favor of clangd, but only on non-Windows systems
]=])
    string(APPEND VSCODE_ENVIRONMENT_PLATFORM_AND_COMPILER_SPECIFIC_CONFIGURATIONS [=[
    "clangd.arguments": [
]=])
    get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)
    string(APPEND VSCODE_ENVIRONMENT_PLATFORM_AND_COMPILER_SPECIFIC_CONFIGURATIONS
"        \"--compile-commands-dir=${BUILD_DIR_NAME}\" // Tell clangd to use compile_commands.json from build directory\n"
    )
    string(APPEND VSCODE_ENVIRONMENT_PLATFORM_AND_COMPILER_SPECIFIC_CONFIGURATIONS [=[
    ]]=])

else()
    message("ERROR: Unsupported compiler-platform combination: can't generate 'settings.json' debug configuration with ${CMAKE_CXX_COMPILER_ID} compiler on ${CMAKE_SYSTEM_NAME}. Not a platform's default toolchain?")
endif()

configure_file(${CMAKE_SOURCE_DIR}/settings.json.in ${CMAKE_SOURCE_DIR}/.vscode/settings.json @ONLY)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU") # TODO Do macOS & BSD need '.clangd'? Or will they work with IntelliSense provided by Microsoft C/C++ VSCode extension? Add conditions and test on these platforms
    message("Configuring '.clangd' for Linux and GCC toolchain")

    get_filename_component(GCC_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
    get_filename_component(GCC_TOOLCHAIN_DIR ${GCC_BIN_DIR} DIRECTORY)

    if(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES)
        message("Using CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES for '-isystem'")
        # Filter for C++ standard library paths
        foreach(include_dir ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
            if(include_dir MATCHES "c\\+\\+")
                string(APPEND CLANGD_SYSTEM_INCLUDES
                    "    - -isystem${include_dir} # Mark directory as the one containing system headers (suppress warnings)\n"
                )
            endif()
        endforeach()
    else()
        message("Using predefined constructed paths for '-isystem'")
        # Fallback to constructed paths
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
            OUTPUT_VARIABLE GCC_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(CLANGD_GCC_INCLUDE_DIR "${GCC_TOOLCHAIN_DIR}/include/c++/${GCC_VERSION}")

        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine
            OUTPUT_VARIABLE GCC_TARGET_TRIPLET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(CLANGD_GCC_TARGET_INCLUDE_DIR "${GCC_TOOLCHAIN_DIR}/include/c++/${GCC_VERSION}/${GCC_TARGET_TRIPLET}")
        set(CLANGD_SYSTEM_INCLUDES
"    - -isystem${CLANGD_GCC_INCLUDE_DIR} # Mark directory as the one containing system headers (suppress warnings)
    - -isystem${CLANGD_GCC_TARGET_INCLUDE_DIR} # Mark directory as the one containing system headers (suppress warnings)"
        )
    endif()

    configure_file(${CMAKE_SOURCE_DIR}/clangd.in ${CMAKE_SOURCE_DIR}/.clangd @ONLY)
endif()

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

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")  # TODO what 'launch.json' do macOS & BSD need? add conditions and test on these platforms
    set(VSCODE_PLATFORM_AND_COMPILER_SPECIFIC_DEBUG_CONFIGURATIONS [=[
"externalConsole": false,
            "MIMode": "lldb"]=])

else()
    message("ERROR: Unsupported compiler-platform combination: can't generate 'launch.json' debug configuration with ${CMAKE_CXX_COMPILER_ID} compiler on ${CMAKE_SYSTEM_NAME}. Not a platform's default toolchain?")
    return()
endif()

configure_file(${CMAKE_SOURCE_DIR}/launch.json.in ${CMAKE_SOURCE_DIR}/.vscode/launch.json @ONLY)

# TODO test on macOS and FreeBSD
