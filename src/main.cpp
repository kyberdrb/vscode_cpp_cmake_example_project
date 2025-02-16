#include <iostream>
#include <print>

int main() {
    int number = 42;
    std::cout << "Hello, VS Code + CMake + clangd!" << std::endl;
    std::cout << number << std::endl;
    std::cout << std::format("Testing std::format") << '\n';
    std::print("Testing std::print");
    return 0;
}
