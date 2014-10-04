/*

The client side application for chatting.

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP


#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include "System.hpp"
#include "../Server/Server.hpp"

namespace chat
{
    class Client
    {
        public:
            Client();
            void loginPrompt();
            bool login();
            bool logout();
            void signup();
            sf::Socket::Status send(sf::Packet& packet);
            bool receive();
            bool isLoggedIn();
            const std::string& getUserName();
            const std::string& getFriendName();

        protected:

        private:
            sf::TcpSocket m_client;
            bool m_loginStatus;
            std::string m_userName;
            //std::vector<std::string> m_friendList;
            std::string m_friend;
            enum class status{
                Available = 0,
                Busy,
                Unavailable,
                Invisible
            } m_onlineStatus;
    };
}

#endif // CLIENT_HPP
