#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>

namespace chat
{
    const int OPEN_PORT = 9999;
    const std::string SERVER_IP_ADDRESS = "127.0.0.1";
    const std::string USER_LIST = "members.dat";
    std::string getCurrentTimeAndDate();
}

#endif // SYSTEM_HPP_INCLUDED
