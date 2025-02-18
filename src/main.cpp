#include <expected>
#include <iostream>
#include <print>
#include <source_location>

// constexpr functions need to be defined before usage - forward declaration is not enough - otherwise compiler throws an error:
//   error: ‘constexpr std::expected<unsigned int, std::__cxx11::basic_string<char> > triggerArtificialException(bool)’ used before its definition
constexpr std::expected<uint32_t, std::string> triggerArtificialException(bool shouldExceptionBeTriggered) {
    if (shouldExceptionBeTriggered) {
        constexpr const std::source_location location = std::source_location::current();
        return std::unexpected(
            std::format("{}:{}:{}:{}: ERROR - Exception occured", 
                location.file_name(), 
                location.function_name(), 
                location.line(),
                location.column() )
            );
    }
    return 42;
}

int main() {
    constexpr int number = 42;
    std::cout << "Hello, VS Code + CMake + clangd!" << std::endl;
    std::cout << number << std::endl;
    std::cout << std::format("Testing std::format") << '\n';
    std::print("Testing std::print\n");

    constexpr auto resultWithValue = triggerArtificialException(false);
    if (resultWithValue.has_value()) {
        std::println("triggerArtificialException(false) [happy path]: {}", resultWithValue.value());
    }

    auto resultWithException = triggerArtificialException(true);
    bool hasException = !(resultWithException.has_value());
    if (hasException) {
        std::println("triggerArtificialException(true) [catching exception]: {}", resultWithException.error());
    }

    return 0;
}
