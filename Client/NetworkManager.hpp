#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <iostream>
#include <SFML/Network.hpp>
#include "System.hpp"

namespace prattle
{
    class NetworkManager
    {
        public:
            NetworkManager();

            void run();
            bool checkIfWhitespace(const std::string& message);
            bool isLoggedIn();
            std::string const& getUserName() const;
            std::vector<std::string> const& getFriendsName() const;
            //bool  receive();
            std::string receive();
            sf::Socket::Status send(sf::Packet& packet);
            bool logout();
            bool login(const std::string& username, const std::string& password);
            void signup(const std::string& username, const std::string& password);
            void reset();

        protected:

        private:
            sf::TcpSocket m_clientSocket;
            bool m_loginStatus;
            std::string m_userName;
            std::string m_password;
            std::vector<std::string> m_friends;
            Status m_onlineStatus;
    };
}

#endif // NETWORK_MANAGER_HPP
