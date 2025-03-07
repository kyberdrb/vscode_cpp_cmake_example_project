#include "FluentBuilder.h"

#include <chrono>
#include <iostream>
#include <print>

#include <algorithm>
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
    constexpr int number = 42;
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

    std::println("");

    auto somethingFluentToBuild = FluentBuilder::create()
        .withNumber(2)
        .withText("Stack Builder is awesome");

    auto start = std::chrono::steady_clock::now();
    std::cout << "std::cout: " << somethingFluentToBuild << '\n';
    auto end = std::chrono::steady_clock::now();
    auto executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::println("std::cout (execution time): {}", executionTime);

    start = std::chrono::steady_clock::now();
    std::println("std::print: {}", somethingFluentToBuild.str());
    end = std::chrono::steady_clock::now();
    executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::println("std::print (execution time): {}", executionTime);

    std::println("");

    // Compiles, but segfaults
    // auto somethingUniquelyFluentToBuild = FluentBuilder::createUnique();
    // auto wup = std::move(somethingUniquelyFluentToBuild)
    //         ->addSomethingUnique(4)
    //         ->addSomethingElseUnique("Builder is awesome");

    // Doesn't compile: operator '->' discards 'rvalue' qualifiers transforming 'rvalue' refs into 'lvalue' refs
    //  Returning unique_ptr is valid, but chaining unique_ptr pointers is ill-formed, as it requires moving ownership back and forth while requiring owning the instance by the client and the built instance at the same time, which would require double deletion
    // auto somethingUniquelyFluentToBuild = std::move(FluentBuilder::createUnique());
    //         ->addSomethingUnique(4)
    //         ->addSomethingElseUnique("Builder is awesome");

    auto somethingUniquelyFluentToBuild = FluentBuilder::createUnique() // auto will first start with unique_ptr...
        ->withUniqueNumber(4) // ...then will convert to lvalue reference...
        .withUniqueText("Unique Builder is awesome"); // ...and stays as lvalue reference

    start = std::chrono::steady_clock::now();
    std::cout << "std::cout: " << somethingUniquelyFluentToBuild << '\n';
    end = std::chrono::steady_clock::now();
    executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::println("std::cout (execution time): {}", executionTime);

    start = std::chrono::steady_clock::now();
    std::println("std::print: {}", somethingUniquelyFluentToBuild.str());
    end = std::chrono::steady_clock::now();
    executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::println("std::print (execution time): {}", executionTime);

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
