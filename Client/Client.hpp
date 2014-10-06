/*

The client side application for chatting.

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include "System.hpp"

namespace chat
{
    class Client
    {
        public:
            Client();

            void loginPrompt();

            void signup();

            bool login();

            bool isLoggedIn();

            sf::Socket::Status send(sf::Packet& packet);

            bool receive();

            const std::string& getUserName();

            const std::string& getFriendName();

            bool logout();

        protected:

        private:
            sf::TcpSocket m_client;
            bool m_loginStatus;
            std::string m_userName;
            std::string m_password;
            //std::vector<std::string> m_friendList;
            std::string m_friend;
            enum class status{
                Available  ,
                Busy       ,
                Unavailable,
                Offline
            } m_onlineStatus;
    };
}

#endif // CLIENT_HPP
