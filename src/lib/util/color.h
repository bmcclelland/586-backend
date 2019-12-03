#pragma once

#include <string>
#include <iosfwd>

namespace mvc {

enum class Color 
{
    off      = 0,
    default_ = 39,
    black    = 30,
    red      = 31,
    green    = 32,
    yellow   = 33,
    blue     = 34,
    magenta  = 35,
    cyan     = 36,
    lgray    = 37,
    gray     = 90,
    lred     = 91,
    lgreen   = 92,
    lyellow  = 93,
    lblue    = 94,
    lmagenta = 95,
    lcyan    = 96,
    white    = 97,
};

enum class BColor
{
    off      = 0,
    default_ = 39,
    black    = 30,
    red      = 31,
    green    = 32,
    yellow   = 33,
    blue     = 34,
    magenta  = 35,
    cyan     = 36,
    lgray    = 37,
    gray     = 90,
    lred     = 91,
    lgreen   = 92,
    lyellow  = 93,
    lblue    = 94,
    lmagenta = 95,
    lcyan    = 96,
    white    = 97,
};

std::string to_string(Color);
std::ostream& operator<<(std::ostream&, Color);

std::string to_string(BColor);
std::ostream& operator<<(std::ostream&, BColor);

} // namespace mvc

