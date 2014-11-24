#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>
#include <SFML/Graphics.hpp>

namespace chat
{
    const int OPEN_PORT = 20000;
    const std::string SERVER_IP_ADDRESS = "127.0.0.1";
    const std::string USER_LIST = "members.dat";
    const std::string getCurrentTimeAndDate();

    enum class ScreenState{
                LoginScreen,
                SignupScreen,
                ChatScreen
            };

    //sf::Color DEFAULT_BG_COLOR = sf::Color::Black;
    //sf::Color DEFAULT_FG_COLOR = sf::Color::White;

    const std::string DEFAULT_GLOBAL_FONT = "resources/fonts/DejaVuSans.ttf";
    const std::string DEFAULT_BACKGROUND = "resources/images/background.png";
    const std::string CHAT_BACKGROUND = "resources/images/background-chat.png";
    const std::string DEFAULT_LOGO = "resources/images/logo.png";
    const std::string DEFAULT_TGUI_THEME = "resources/widgets/Black.conf";
}

#endif // SYSTEM_HPP_INCLUDED
