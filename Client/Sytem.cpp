#include "System.hpp"

namespace prattle
{
    const std::string getCurrentTimeAndDate()
    {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);

        strftime(buf, sizeof(buf), "%r, %a %d %b %Y", &tstruct);

        return buf;
    }
}
