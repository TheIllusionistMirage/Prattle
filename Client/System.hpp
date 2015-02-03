/**

    Prattle/Client/System.hpp

    Holds various constants, enumerations for use by the Client side application.

*/


#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>
#include "../include/ErrorLog.hpp"

namespace prattle
{
    const int OPEN_PORT = 19999;
    const std::string SERVER_IP_ADDRESS = "127.0.0.1";
    const std::string getCurrentTimeAndDate();

    enum class ScreenState{
                LoginScreen ,
                SignupScreen,
                ChatScreen
            };

    enum class Status{
                Online  ,
                Busy    ,
                Offline
            };

    //sf::Color DEFAULT_BG_COLOR = sf::Color::Black;
    const std::string DEFAULT_GLOBAL_FONT = "resources/fonts/DejaVuSans.ttf";
    const std::string DEFAULT_BACKGROUND = "resources/images/new_bg.png";
    //const std::string CHAT_BACKGROUND = "resources/images/background-chat.png";
    const std::string CHAT_BACKGROUND = "resources/images/new_bg.png";
    const std::string DEFAULT_LOGO = "resources/images/logo.png";
    const std::string DEFAULT_TGUI_THEME = "resources/widgets/Black.conf";
}

#endif // SYSTEM_HPP_INCLUDED
