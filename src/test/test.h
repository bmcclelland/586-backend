#pragma once

#include "util/color.h"
#include "util/containers.h"
#include "util/string.h"
#include <iostream>

namespace mvc::test {

#define TEST(name, ...)\
    bool test_stub_##name = []()\
    {\
        tests().push_back([]()\
            {\
                std::cout << BColor::white << #name << Color::off << ".. ";\
                __VA_ARGS__\
                std::cout << BColor::green << "PASSED" << Color::off << "\n";\
            });\
        return true;\
    }();\
// TEST

#define TEST_TRUE(b)\
    try\
    {\
        if (!(b))\
        {\
            fail(#b, "true", "false");\
        }\
    }\
    catch (std::exception& e)\
    {\
        fail(#b, "true", std::string("exception \"") + e.what() + "\"");\
    }\
// TEST_TRUE

#define TEST_EQ(a,b)\
    try\
    {\
        if (!((a) == (b)))\
        {\
            fail(#a, b, a);\
        }\
    }\
    catch (std::exception& e)\
    {\
        fail(#a, #b, std::string("exception \"") + e.what() + "\"");\
    }\
// TEST_EQ

#define TEST_THROWS(b)\
    try\
    {\
        fail(#b, "exception", b);\
    }\
    catch (...)\
    {}\
// TEST_THROWS

using TestFn = void(*)();

inline
Vec<TestFn>& tests()
{
    static Vec<TestFn> data;
    return data;
}

template <typename E, typename R>
void fail(std::string const& test, E const& expected, R const& result)
{
    std::cout << BColor::red 
              << "FAILED" 
              << BColor::yellow
              << "\n    Test:     "
              << Color::off
              << test
              << BColor::yellow
              << "\n    Expected: " 
              << Color::off
              << expected
              << BColor::yellow
              << "\n    Result:   "
              << Color::off
              << result
              << std::endl;
    std::exit(1);
}

inline
void run_all()
{
    std::cout << BColor::yellow << "RUNNING ALL TESTS\n" << Color::off;

    for (auto test : tests())
    {
        test();
    }

    std::cout << BColor::green << "ALL TESTS OK\n" << Color::off;
}

} // namespace mvc::test
