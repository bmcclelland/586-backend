#pragma once

#include "util/color.h"
#include "util/newtype.h"
#include <iostream>

namespace mvc 
{
    template <typename... Ts>
    void print(Ts const&... ts)
    {
        (std::clog << ... << unwrap_newtype(ts));
    }

    template <typename... Ts>
    void println(Ts const&... ts)
    {
        print(ts...);
        std::clog << std::endl;
    }

    template <typename... Ts>
    void print_notice(BColor color, char const* module_id, Ts const&... ts)
    {
        std::clog << Color::off << "[" << color << module_id << Color::off << "] ";
        println(ts...);
    }
}

#define PRINT(...) print(__VA_ARGS__)
#define PRINTLN(...) println(__VA_ARGS__)

#define PRINT_NOTICE(...) print_notice(BColor::MODULE_COLOR, MODULE_ID, __VA_ARGS__);
#define PRINT_ERROR(...) print_notice(BColor::red, MODULE_ID, __VA_ARGS__);
