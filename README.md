# VSCode CMake C++ project template

Why? Because my CLion license expired.  

This is an introductory project to the realm of cross-platform, desktop, open-source C++ development.

Tested on:

- Arch Linux with GCC 14.2.1
- Windows 11 with MSVC 19.43.34810

## The Path of Least Resistance - Simplified Guide

We're only going to build the Debug version of the project with the default toolchain, default compiler and default makefile generator for each platform, to make things as simple as possible.

### System Environment preparation - Installing the Toolchain

Click on the operating system you want to build the project on, to jump to the platform specific instructions for installing necessary tools.

- [Windows 11](#windows-11---tools-installation) - MSVC toolchain
- [Linux](#linux---tools-installation) - GCC toolchain
- [macOS](#macos---tools-installation) - Apple LLVM / Apple Clang toolchain

### Windows 11 - tools installation

Install following tools:

- [Visual Studio 2022 Community](https://visualstudio.microsoft.com/vs/community/) - provides the MSVC compiler, _Visual Studio 17 2022_ makefile generator for CMake, CMake build system utilities and _Developer PowerShell for VS 2022_ (which isn't included in _Build Tools for Visual Studio_ unfortunately, therefore we're installing the entire Visual Studio IDE).
    - Install Visual Studio 2022 Community IDE. Open _Visual Studio Installer_, where you choose following components for download and installation:
    
        **_Core Components:_**

        - MSVC Compiler Toolchain in latest version (at the time of writing it's v143) in both variants: with and without Spectre/Meltdown mitigations
        - Windows SDK (Windows 11 24H2 uses `Windows SDK version 10.0.22621.0 to target Windows 10.0.26100` as seen in the CMake generate output)
        - C++ Core Features (includes standard library implementations)
        - CMake tools for Windows
- [git](https://git-scm.com/downloads/win) - _git_ version control system
    - Install with default settings.

#### Adjusting the system

Verify that the `git` executable recognizable by the system without specifying the exact path to the `git.exe` executable: Open _Developer PowerShell for VS 2022_ and type there `git --version` . If you get a response similar to

```
git version 2.49.0.windows.1
```

everything is set up correctly, and we can proceed with building the project.

Otherwise it's needed put the binary directory in the system `Path` variable.

1. Open Windows Settings (e.g. Win+X -> Settings) -> System -> About -> Advanced system settings . A dialog window _System Properties_ opens.
1. In the _System Properties_ dialog window click on tab _Advanced_ -> click on _Environment Variables_ button. A dialog window _Environment Variables_ opens.
1. In the _Environment Variables_ dialog window In section _System variables_ find variable `Path` . Double click on it. A dialog window _Edit environment variable opens_.
1. In the _Edit environment variable opens_ dialog window click on _New_ button. A new line at the bottom in the list of paths is created. Assuming the git is installed to the default path `C:\Program Files\Git`, enter in the line the `cmd` directory in it, where the `git.exe` is located - `C:\Program Files\Git\cmd` [note the absence of the trailing backslash!] . Confirm all dialog windows by clicking on _OK_ button.
1. Open a new _Developer PowerShell for VS 2022_ window and test the `git --version` command again. Now the terminal will recognize the `git` as a valid command and outputs the currently installed git version as noted above.

Continue with [building the project](#windows-11---clone--build-instructions).

### Linux - tools installation

Install packages that contain these utilities which provide following functionality:

- `gcc`/`g++` - The Compiler
- `make` - _Unix Makefiles_ generator for CMake
- `cmake` - CMake metamakefiles generator
- `git` - _git_ version control system

All these packages might be contained in one metapackage `build-essentials` , that might be available for your Linux distribution. For brewity and simplicity of this tutorial, I'll leave that for you as a homework, how to install the toolchain

### macOS - tools installation

Install following tools

- [Xcode](https://apps.apple.com/us/app/xcode/id497799835?mt=12/) - provides Apple Clang compiler, _Xcode_ makefile generator and Xcode Command Line Tools
- [CMake](https://github.com/Kitware/CMake/releases/download/v4.0.1/cmake-4.0.1-macos10.10-universal.dmg) - provides CMake build system utilities

### VSCode Extensions

**_VSCode extensions for Windows_**

![](res/vscode_extensions-windows.png)

**_VSCode extensions for Linux_** TODO update: CMake from twxs is no longer needed

![](res/vscode_extensions-linux.png)

- [`C/C++`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) from _Microsoft_
    - adds support for debugging and syntax completion
- [`CMake Tools`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) from _Microsoft_
    - adds support for CMake syntax completion & highlighting
    - installs `CMake` [twxs] extension alongside with the main extension, but since version ~1.21.13 it's no longer needed, thus can be disabled - for syntax completion and referencing
- **_[Linux only!]_** [`clangd`](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) from _LLVM_
    - optional; adds support for syntax completion - `clangd` completion seems to me to be more reliable on Linux platform than default Intellisense from Microsoft's `C/C++` extension; maybe on macOS it works the same way. Maybe not on all Linux distributions, but I use it in VSCode on Arch Linux.

### Build instructions - The Path of Least Resistance

We're only going to build the Debug version of the project with default compiler and makefile generator, to make things as simple as possible.

- Configuring with CMake
- Building the project
- Running the executable

Click on the operating system you want to build the project on, to jump to the platform specific instructions for building the project.

- [Windows 11](#windows-11---clone--build-instructions): MSVC toolchain
- [Linux](#linux---clone--build-instructions): GCC toolchain
- [macOS](#macos---clone--build-instructions): Apple LLVM / Apple Clang toolchain

#### Windows 11 - clone & build instructions

Open _Developer PowerShell for VS 2022_

##### Full build

You may copy&paste the commands line-by-line, or execute it all at once, in the _Developer PowerShell for VS 2022_

```powershell
cd "${HOME}"
mkdir git
cd git
git clone https://github.com/kyberdrb/vscode_cpp_cmake_example_project.git
cd vscode_cpp_cmake_example_project

# Error 'Cannot find path' is normal, when the directory doesn't exist. This command runs successfully when the directory will be present after building the project
Remove-Item -Recurse -Verbose -Path build

mkdir build # fails, if the directory already exists, which is also normal
cd build

ls .. # look at the files we're going to work with
cmake .. #TODO not 'cmake -DCMAKE_BUILD_TYPE=Debug ..' # Generate build files via CMake from the directory containing CMakeLists.txt with the use of the default toolchain

ls # look at the files we're going to work with
cmake --build . # Build the project from the generated build files which creates executable file
./Debug/my_cpp_project.exe # Run the project
```

[Here](res/example_output-windows_11-simple.txt) is an example output from _Developer PowerShell for VS 2022_ on Windows 11 with Visual Studio 17.13.6 and Git 2.49.0 .

Example output from _Developer PowerShell for VS 2022_ on Windows 11 with Visual Studio 2022 Community 17.13.6 and Git 2.49.0 is available [here](res/example_output-simple_windows_11.txt).

##### Incremental build

After making changes to the project, we don't need to go through the entire procedure. This one-liner command in the _Developer PowerShell for VS 2022_ is sufficient for the changes to be built into the executable.

```powershell
cd "${HOME}\git\vscode_cpp_cmake_example_project\build\" ; cmake .. ; cmake --build . ; .\Debug\my_cpp_project.exe
```

#### Linux - clone & build instructions

Open _Terminal_

##### Full build

```sh
cd "${HOME}"
mkdir git
cd git
git clone https://github.com/kyberdrb/vscode_cpp_cmake_example_project.git
cd vscode_cpp_cmake_example_project

mkdir build-Debug
cd build-Debug

rm --recursive --verbose build-Debug

ls ..
cmake -DCMAKE_BUILD_TYPE=Debug .. // a little more complex than 'cmake ..' but the option '-DCMAKE_BUILD_TYPE=Debug' adds debug information into the executable, making it debuggable with 'F5' found under 'Run & Debug' (Ctrl + Shift + D) on the side menu

ls
cmake --build .

./my_cpp_project
```

##### Incremental build

```sh
cd .. && rm -r build-Debug/ && mkdir build-Debug/ # optional: for rebuilds
date && cd "${HOME}/git/vscode_cpp_cmake_example_project/build-Debug" && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . && ./my_cpp_project
```

#### macOS - clone & build instructions

Open _Terminal_

```
TODO build commands
```

##### Incremental build

```
TODO build command
```

### Making Changes

**Edit-Build-Run workflow**  
After making changes to the project, build it incrementally by using the command in the _Incremental build_ chapters.

Command for running the compiled executable normally - without debugging - is listed in the build instructions for specific platforms as the last command.

Debugging sessions can be configured and launched in _Run & Debug_ (`Ctrl + Shift + D`) extension pane on the left hand side. At the top of the pane, select the debugging session by your platform, i.e. operating system: these options are configured in `launch.json` . In the `main.cpp` click on the line numbers you want to stop the program's execution and introspect the variable values. Then start the debugging session by clicking on the green triangle or by pressing `F5` .

## VSCode Project configuration

- settings.json
- .clangd

## Build Instructions - The Path of Ninja - Advanced Guide

Are you ready for more? Check out advanced stuff on branch `advanced_skeleton` coming soon!

## Sources

See the [`sources` file](sources.md) .
