/*

The client side application for chatting.

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP


#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <TGUI/TGUI.hpp>
#include "System.hpp"

namespace chat
{
    class Client
    {
        public:
            Client();
            void loginPrompt();
            /* The guest's username and password have been hardcoded. */
            bool login();//const std::string& username = "Guest", const std::string& password = "guest");
            bool logout();
            void signup();
            sf::Socket::Status send(sf::Packet& packet);
            bool isLoggedIn();

        protected:

        private:
            sf::TcpSocket m_client;
            bool m_loginStatus;

    };
}

#endif // CLIENT_HPP
