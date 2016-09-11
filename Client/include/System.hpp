/**

    Prattle/Client/System.hpp
    =========================

    Holds various constants, enumerations for use by the Client side application.

*/


#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#if defined (_WIN32) && (defined (__MINGW32__) || defined (__MINGW64__))
    #include <sstream>

    template <typename T>
    std::string to_string(T t)
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

    int stoi(std::string str);
#else
    #include <string>
    using std::to_string;
    using std::stoi;
#endif

#include <string>
#include "../include/Logger.hpp"

namespace prattle
{
    // client.conf contains IP address of server and the open port
    const std::string CLIENT_CONFIG_FILE = "resources/local/client.conf";
    const std::string prattle_logo =
        "           , | .\n"
        "         ' | | | ! .\n"
        "         | | | | | | .\n"
        "         | | | | | | | |\n"
        "         | | | | | | | | | ' .\n"
        "         | | | | | | | | | | | | | | ' .\n"
        "         | | | | | . ' ' | | | . '\n"
        "         | | ! . '         '\n"
        "         | ! '\n"
        "         |\n"
        "                              ||   ||  ||\n"
        " . .====.   . .===   .====. -====~====-||   .====.\n"
        " |/      \\. |/     ./`     \\  ||   ||  || ./`     \\\n"
        " ||      || ||     !|      || ||   ||  || ||~~~~~~'\n"
        " |\\     ,/' ||     `\\     ,|| ||   ||  || `\\      ,\n"
        " ||`==='`   ''       `'==` '' ''   ''  ''   `'==='\n"
        " ||                               Always be near";
    std::string getCurrentTimeAndDate();
}

#endif // SYSTEM_HPP_INCLUDED
