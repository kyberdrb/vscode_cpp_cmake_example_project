#include <algorithm>
#include <chrono>
#include <expected>
#include <iostream>
#include <print>
#include <source_location>
#include <vector>

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
    static constexpr int number = 42;
    std::cout << "Hello, VS Code + CMake + clangd!" << std::endl;
    std::cout << number << std::endl;
    std::cout << std::format("Testing std::format") << '\n';
    std::print("Testing std::print\n");

    std::println("");

    constexpr auto resultWithValue = triggerArtificialException(false);
    if (resultWithValue.has_value()) {
        std::println("triggerArtificialException(false) [happy path]: {}", resultWithValue.value());
    }

    std::println("");

    auto resultWithException = triggerArtificialException(true);
    bool hasException = !(resultWithException.has_value());
    if (hasException) {
        std::println("triggerArtificialException(true) [catching exception]: {}", resultWithException.error());
    }

    std::println("");

    std::vector numbers{1, 2, 3};

    auto isElementFound = std::any_of(numbers.begin(), numbers.end(), 
        [](auto currentNumber){
            return currentNumber == 2;
        } );

    if (isElementFound) {
        std::println("std::any_of found an element matching criteria");
    }

    std::println();

    std::println(
        "{}/{}/{}", 
        std::chrono::year(), 
        std::chrono::month(), 
        std::chrono::day()
    );

    std::println("{:%Y/%m/%d}", std::chrono::system_clock::now());

    auto currentTimepoint = std::chrono::system_clock::now();
    // auto currentTime = std::chrono::steady_clock::now();
    auto date = std::chrono::year_month_day{
        std::chrono::floor<std::chrono::days>(currentTimepoint)
    };

    std::println(
        "{}/{:02}/{}", 
        date.year(), 
        date.month().operator unsigned(), 
        date.day()
    );

    std::println("{:%Y/%m/%d}", date);
    //constexpr
    auto yearMonthDay{std::format("{:%Y/%m/%d}", date)};
    std::println("{}", yearMonthDay);

    auto currentTimepointInCurrentTimezone = std::chrono::zoned_time{std::chrono::current_zone(), currentTimepoint};
    std::println("{:%Y/%m/%d}", currentTimepointInCurrentTimezone.get_local_time()); 

    // Type-safe date construction
    constexpr auto thanksgiving = std::chrono::year(2025)/11/27;
    //static constexpr auto thanksgiving = std::chrono::year(2025)/11/27; // Jason Turner: cppweekly Ep. 312, 315, 339
    std::println("Thanksgiving: {:%Y/%m/%d}", thanksgiving);

    // Formatting for column alignment with the next line
    std::println("{:<23}{:%Y/%m/%d}", "Thanksgiving:", thanksgiving);

    // Intuitive way, but doesn't work - maybe make own 'std::date' class as a proposal for 'intuitive' way of working with date and time
    // auto thanksgiving = std::chrono::year_month_day{"2025/11/27"};
    // std::println("Thanksgiving: {:%Y/%m/%d}", thanksgiving);

    // Proper parsing from string
    std::istringstream iss{"2025/10/13"};
    std::chrono::year_month_day canadianThanksgiving;
    iss >> std::chrono::parse("%Y/%m/%d", canadianThanksgiving);

    // Safer production version with error checking
    if(iss.fail()) {
        std::println("Failed to parse date!");
        return 1;
    }

    // Verified output
    std::println("Canadian Thanksgiving: {:%Y/%m/%d}", canadianThanksgiving);

    std::println("EOF");

    return 0;
}
