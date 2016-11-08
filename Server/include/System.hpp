/*************************************************************
 *                                                           *
 *                Prattle/Server/System.hpp                  *
 *                                                           *
 *    ===================================================    *
 *                                                           *
 *    Contains various constants and helpers                 *
 *    related to handling the server.                        *
 *                                                           *
 *    See https://github.com/TheIllusionistMirage/Prattle    *
 *    for more details.                                      *
 *                                                           *
 *************************************************************/


#ifndef SYSTEM_HPP
#define SYSTEM_HPP

// Workaround for MinGW versions missing to_string & stoi

#include <string>

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
    using std::to_string;
    using std::stoi;
#endif

#include <random>
#include "../include/SHA256/SHA256.h"
#include "../include/Logger.hpp"

namespace prattle
{
    /* Constants */

    // Server config file
    const std::string SERVER_CONFIG_FILE = "resources/server.conf";

    // User database
    const std::string USER_LIST          = "resources/members.db";

    // Prattle's ASCII logo, courtesy of amhndu
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

    /* Helpers */

    // Returns the current date & time in the host machine
    std::string   getCurrentTimeAndDate();

    // Generates a salt to add to the hashed password
    std::string   generate_salt(int length = 5);

    // Hashes the password using the SHA256
    std::string   pwd_hash(const std::string& str,const std::string& salt);
}

#endif // SYSTEM_HPP_INCLUDED
