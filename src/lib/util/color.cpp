#include "color.h"

namespace mvc {
   
#ifdef _WIN32
#undef MVC_COLOR
#else
#define MVC_COLOR
#endif

#ifdef MVC_COLOR

    std::string to_string(Color c) 
    {
        return "\033[0;" 
            + std::to_string(static_cast<int>(c)) 
            + "m";
    }

    std::string to_string(BColor c) 
    {
        return "\033[1;" 
            + std::to_string(static_cast<int>(c)) 
            + "m";
    }

#else // ifdef MVC_COLOR

    std::string to_string(Color c) 
    {
        return "";
    }

    std::string to_string(BColor c) 
    {
        return "";
    }

#endif // ifdef MVC_COLOR

std::ostream& operator<<(std::ostream& s, Color c)
{
    return s << to_string(c);
}

std::ostream& operator<<(std::ostream& s, BColor c)
{
    return s << to_string(c);
}

} // namespace mvc

