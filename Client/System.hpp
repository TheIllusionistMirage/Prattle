#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>

namespace chat
{
    const int OPEN_PORT = 9999;
    const std::string SERVER_IP_ADDRESS = "127.0.0.1";
    const std::string getCurrentTimeAndDate();

    enum class OnlineStatus{
                Available  ,
                Busy       ,
                Unavailable,
                Offline
            };

    enum class ScreenState{
                LoginScreen,
                SignUpScreen,
                ChatScreen
            };

    const std::string LOCAL_INFO = "resources/local/local.dat";

    const std::string DEFAULT_GLOBAL_FONT = "resources/fonts/DejaVuSans.ttf";
    const std::string DEFAULT_BACKGROUND = "resources/images/background.png";
    const std::string DEFAULT_LOGO = "resources/images/logo.png";
    const std::string DEFAULT_TGUI_THEME = "resources/widgets/Black.conf";
}

#endif // SYSTEM_HPP_INCLUDED
