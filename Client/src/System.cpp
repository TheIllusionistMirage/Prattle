/*************************************************************
 *                                                           *
 *                Prattle/Client/System.cpp                  *
 *                                                           *
 *    ===================================================    *
 *                                                           *
 *    Contains implementations of helpers declared in        *
 *    Prattle/Client/System.hpp.                             *
 *                                                           *
 *    See https://github.com/TheIllusionistMirage/Prattle    *
 *    for more details.                                      *
 *                                                           *
 *************************************************************/


#if defined (_WIN32) && (defined (__MINGW32__) || defined (__MINGW64__))
    #include <sstream>

    // Declared in Prattle/Server/System.hpp

    int stoi(std::string str)
    {
        int i;
        std::stringstream ss(str);
        ss >> i;
        return i;
    }
#endif

#include "../include/System.hpp"
#include <ctime> // for time

namespace prattle
{
    // By following http://www.cplusplus.com/reference/ctime/strftime/
    std::string getCurrentTimeAndDate()
    {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%I:%m:%S %p, %a %d %b %Y", &tstruct);
        return buf;
    }
}
