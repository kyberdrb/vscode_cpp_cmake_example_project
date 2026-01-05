#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#ifndef __APPLE__ // tested on macOS 11.7.1 Big Sur: AppleClang 12.0.5
#include <expected>
#include <print>
#include <ranges>
#include <source_location>
#endif

#ifndef __APPLE__
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
#endif

// Test 1: Basic variable declarations with different initialization styles
void test_initialization_styles() {
    // Assignment initialization
    int assignInt = 42;
    double assignDouble = 3.14;
    std::string assignString = "assign";
    std::vector<int> assignVec = {1, 2, 3};

    // Uniform initialization (brace initialization)
    int braceInt{42};
    double braceDouble{3.14};
    std::string braceString{"brace"};
    std::vector<int> braceVec{1, 2, 3};

    // Direct initialization (parentheses)
    int parenInt(42);
    double parenDouble(3.14);
    std::string parenString("paren");
    std::vector<int> parenVec(3, 5);  // 3 elements of value 5

    // Copy initialization with explicit constructor
    std::string explicitString = std::string("explicit");
    std::vector<int> explicitVec = std::vector<int>{7, 8, 9};

    // Test references to each variable
    assignInt++;
    braceInt++;
    parenInt++;

    assignDouble += 1.0;
    braceDouble += 1.0;
    parenDouble += 1.0;

    assignString += " modified";
    braceString += " modified";
    parenString += " modified";
    explicitString += " modified";

    assignVec.push_back(4);
    braceVec.push_back(4);
    parenVec.push_back(4);
    explicitVec.push_back(10);
}

// Test 2: Auto with different initialization styles
void test_auto_initialization_styles() {
    // Auto with assignment
    auto autoAssignInt = 42;
    auto autoAssignDouble = 3.14;
    auto autoAssignString = std::string("auto assign");

    // Auto with brace initialization
    auto autoBraceInt{42};        // int (not initializer_list!)
    auto autoBraceDouble{3.14};   // double
    auto autoBraceString{std::string("auto brace")};

    // Auto with parentheses initialization (C++20)
    auto autoParenInt = int(42);
    auto autoParenDouble = double(3.14);

    // Auto with explicit type
    auto autoExplicitVec = std::vector<int>{1, 2, 3};

    // Reference each variable
    autoAssignInt++;
    autoBraceInt++;
    autoParenInt++;

    autoAssignString += " used";
    autoBraceString += " used";

    autoExplicitVec.push_back(4);
}

// Test 3: Container initialization variations
void test_container_initialization() {
    // Vector - assignment with initializer list
    std::vector<int> vecAssign = {10, 20, 30, 40};

    // Vector - brace initialization
    std::vector<int> vecBrace{10, 20, 30, 40};

    // Vector - constructor with size
    std::vector<int> vecSize(5);  // 5 default-initialized elements

    // Vector - constructor with size and value
    std::vector<int> vecFilled(5, 100);  // 5 elements of value 100

    // Map - different initialization styles
    std::map<std::string, int> mapAssign = {{"a", 1}, {"b", 2}};
    std::map<std::string, int> mapBrace{{"x", 10}, {"y", 20}};

    // Test references
    vecAssign[0] = 999;
    vecBrace.push_back(50);
    vecSize[0] = 7;
    vecFilled[0] = 200;

    mapAssign["c"] = 3;
    mapBrace["z"] = 30;
}

// Test 4: Class members with different initialization
class TestClass {
public:
    // Default member initialization
    int memberDefault = 100;
    std::string stringDefault{"default"};

    // Member variables initialized in constructor
    int memberCtorAssign;
    int memberCtorBrace;
    std::string stringCtorAssign;
    std::string stringCtorBrace;

    // Constructor with different initialization styles
    TestClass(int a, int b, std::string s1, std::string s2) :
        memberCtorAssign(a), // Parentheses initialization
        memberCtorBrace{b}, // Brace initialization
        stringCtorAssign(s1),
        stringCtorBrace{s2}
    {
        // Use member variables
        memberDefault++;
        memberCtorAssign++;
        memberCtorBrace++;

        stringDefault += " used";
        stringCtorAssign += " used";
        stringCtorBrace += " used";
    }
};

// Test 5: Pointer and smart pointer initialization styles
void test_pointer_initialization() {
    // Raw pointers
    int* rawPtrAssign = new int(42);
    int* rawPtrBrace = new int{42};

    // Smart pointers - shared_ptr
    std::shared_ptr<int> sharedAssign = std::make_shared<int>(100);
    auto sharedAuto = std::make_shared<int>(100);

    // Smart pointers - unique_ptr
    std::unique_ptr<int> uniqueAssign = std::make_unique<int>(200);
    auto uniqueAuto = std::make_unique<int>(200);

    // Shared_ptr with brace initialization
    std::shared_ptr<std::string> sharedString = std::make_shared<std::string>("test");

    // Test references
    *rawPtrAssign = 50;
    *rawPtrBrace = 51;
    *sharedAssign = 150;
    *sharedAuto = 151;
    *uniqueAssign = 250;
    *uniqueAuto = 251;
    *sharedString = "modified";

    delete rawPtrAssign;
    delete rawPtrBrace;
}

// Test 6: Lambda captures with different styles
void test_lambda_initialization() {
    int captureAssign = 42;
    int captureBrace{42};

    // Lambda with copy capture
    auto lambdaCopy = [captureAssign, captureBrace]() {
        return captureAssign + captureBrace;
    };

    // Lambda with reference capture
    auto lambdaRef = [&captureAssign, &captureBrace]() {
        captureAssign++;
        captureBrace++;
        return captureAssign + captureBrace;
    };

    // Lambda with init capture (C++14)
    auto lambdaInit = [localAssign = captureAssign, localBrace{captureBrace}]() {
        return localAssign + localBrace;
    };

    lambdaCopy();
    lambdaRef();
    lambdaInit();
}

// Test 7: Const and constexpr with different initialization
void test_const_initialization() {
    // Const with different styles
    const int constAssign = 42;
    const int constBrace{42};
    const int constParen(42);

    // Constexpr with different styles
    constexpr int constexprAssign = 100;
    constexpr int constexprBrace{100};

    // Auto const
    const auto autoConstAssign = 42;
    const auto autoConstBrace{42};

    // Test references (read-only)
    int useConst = constAssign + constBrace + constParen;
    int useConstexpr = constexprAssign + constexprBrace;
    int useAutoConst = autoConstAssign + autoConstBrace;
}

// Test 8: Reference initialization styles
void test_reference_initialization() {
    int original = 42;

    // Lvalue references - different styles
    int& refAssign = original;
    int& refBrace{original};

    // Const references
    const int& constRefAssign = original;
    const int& constRefBrace{original};

    // Auto references
    auto& autoRefAssign = original;
    auto& autoRefBrace{original};

    // Rvalue references
    int&& rvalueAssign = 100;
    int&& rvalueBrace{200};

    // Test modifications
    refAssign = 50;
    refBrace = 51;
    autoRefAssign = 52;
    autoRefBrace = 53;
    rvalueAssign = 150;
    rvalueBrace = 250;
}

// Test 9: Structured bindings with different initialization
void test_structured_binding_initialization() {
    // Pair initialization - assignment
    std::pair<int, std::string> pairAssign = {1, "one"};
    auto [valueAssign, stringAssign] = pairAssign;

    // Pair initialization - brace
    std::pair<int, std::string> pairBrace{2, "two"};
    auto [valueBrace, stringBrace] = pairBrace;

    // Direct structured binding
    //auto [directValue, directString] = std::make_pair(3, "three");
    auto [directValue, directString] = std::make_pair(3, std::string{"three"});

    // Test references
    valueAssign++;
    valueBrace++;
    directValue++;

    stringAssign += " modified";
    stringBrace += " modified";
    directString += " modified";    //error: invalid operands to binary expression ('std::__1::tuple_element<1, std::__1::pair<int, const char *>>::type' (aka 'const char *') and 'const char [10]')
                                    //directString += " modified";
                                    //~~~~~~~~~~~~ ^  ~~~~~~~~~~~
                                    // when the structured binding is defined as: 'auto [directValue, directString] = std::make_pair(3, "three");'
                                    // solved by explicitly defining the type of the string literal: 'auto [directValue, directString] = std::make_pair(3, std::string{"three"});'
}

int main() {
    static constexpr int number = 42;
    std::cout << "Hello, VS Code + CMake + clangd!" << std::endl;
    std::cout << number << std::endl;

    test_initialization_styles();
    test_auto_initialization_styles();
    test_container_initialization();

    TestClass obj(10, 20, "assign", "brace");

    test_pointer_initialization();
    test_lambda_initialization();
    test_const_initialization();
    test_reference_initialization();
    test_structured_binding_initialization();

    #ifndef __APPLE__
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

    std::vector numbers{1, 2, 3, 4};

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

    //std::println("{:%Y/%m/%d}", std::chrono::system_clock::now()); // Can produce a red-squiggly underline reporting a false-positive ill-formed expression by '"C_Cpp.intelliSenseEngine": "default"' saying: "call to consteval function "std::basic_format_string<_CharT, _Args...>::basic_format_string(const _Ty &_Str_val) [with _CharT=char, _Args=<std::chrono::system_clock::time_point>, _Ty=char [12]]" did not produce a valid constant expressionC/C++(3133)"

    // below are ways of using C++20 chrono calendar features that are properly recognized by '"C_Cpp.intelliSenseEngine": "default"'
    std::string formatted_time = std::format("{:%Y/%m/%d}", std::chrono::system_clock::now());
    std::cout << formatted_time << std::endl;
    std::println("{}", formatted_time);

    auto currentTimepoint = std::chrono::system_clock::now();
//     // auto currentTime = std::chrono::steady_clock::now(); // 'std::chrono::steady_clock' doesn't work for chrono C++20 calendar features
    auto date = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(currentTimepoint)};
    std::println("{:%Y/%m/%d}", date);

#ifndef _MSC_VER
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_data = std::localtime(&time); // Triggers warning under MSVC 19.44.35211.0: src\main.cpp(85,29): warning C4996: 'localtime': This function or variable may be unsafe. Consider using localtime_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
    std::cout << std::put_time(tm_data, "%Y/%m/%d") << std::endl;
    std::stringstream tm_data_as_sstream;
    tm_data_as_sstream << std::put_time(tm_data, "%Y/%m/%d");
    std::println("{}", tm_data_as_sstream.str());
#endif

#ifdef _MSC_VER
    auto now2 = std::chrono::system_clock::now();
    std::time_t time2 = std::chrono::system_clock::to_time_t(now2);
    std::tm tm_data2;  // Create a struct, not a pointer
    errno_t result2 = localtime_s(&tm_data2, &time2);  // Note the parameter order
    if (result2 == 0) {
        std::cout << std::put_time(&tm_data2, "%Y/%m/%d") << std::endl;
        std::stringstream tm_data_as_sstream2;
        tm_data_as_sstream2 << std::put_time(&tm_data2, "%Y/%m/%d");
        std::println("{}", tm_data_as_sstream2.str());
    } else {
        std::cerr << "Error converting time" << std::endl;
    }
#endif

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

#ifdef _MSC_VER
    std::println("{}{}", "Even numbers: ", numbers | std::views::filter([](auto number) {return number % 2 == 0;}));
#endif

    auto print_subrange = [](std::ranges::viewable_range auto&& r)
    {
        std::cout << '[';
        for (char space[]{0,0}; auto elem : r)
            std::cout << space << elem, *space = ' ';
        std::cout << "] ";
    };

    const auto v = {1, 2, 3, 4, 5, 6};
 
    std::cout << "All sliding windows of width:\n";
    for (const unsigned width : std::views::iota(1U, 1U + v.size()))
    {
        auto const windows = v | std::views::slide(width);
        std::cout << "W = " << width << ": ";
        std::ranges::for_each(windows, print_subrange);
        std::cout << '\n';
    }

    auto windowLength{3};
    auto const windows = v | std::views::slide(windowLength);
    std::print("{}", "W=3: ");
    std::ranges::for_each(windows, print_subrange);
    std::println();
    auto firstWindow = *std::ranges::begin(windows);
    std::println("{}", "first window: ");

    for (const auto& element : firstWindow) {
        std::println("{}", element);
    }

    std::ranges::for_each(firstWindow.cbegin(), firstWindow.cend(), [](auto&& element) {
        std::println("{}", element);
    });

    std::println("{}{}", "first element of first window: ", *firstWindow.cbegin());
    std::println("{}{}", "last element of first window: ", *std::prev(firstWindow.cend()));

    std::println("{}", "---");
    for (const auto& subrange : windows) {
        for (const auto& subrangeElement : subrange) {
            std::println("{}", subrangeElement);
        }

        std::println("{}{}", "first element in this subrange: ", *subrange.cbegin());
        std::println("{}{}", "last element in this subrange: ", *std::prev(subrange.cend()));
        std::println("{}", "---");
    }

    std::println("EOF");

    // Uncomment to test (integrated VSCode) terminal input
    //  to verify potential restrictions due to warning when launching debug session
    //    &"warning: GDB: Failed to set controlling terminal: Operation not permitted\n"
    //int num{};
    //std::cin >> num;
    //std::println("{}", num);

    #endif

    return 0;
}
