/**

    Prattle/Client/System.hpp
    =========================

    Holds various constants, enumerations for use by the Client side application.

*/


#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>
#include "../include/ErrorLog.hpp"

namespace prattle
{
    // client.conf contains IP address of server and the open port
    const std::string CLIENT_CONFIG_FILE = "resources/local/client.conf";
    const std::string getCurrentTimeAndDate();

    enum class ScreenState
    {
        LoginScreen ,
        SignupScreen,
        ChatScreen
    };

    enum class Status
    {
        Online  ,
        Busy    ,
        Offline
    };
}

#endif // SYSTEM_HPP_INCLUDED
