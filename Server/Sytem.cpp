#include "System.hpp"

namespace chat
{
    std::string getCurrentTimeAndDate()
    {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);

        strftime(buf, sizeof(buf), "%r, %a %d %b %Y", &tstruct);

        return buf;
    }
    std::string pwd_hash(const std::string& str)
    {
        return str;
    }
}
