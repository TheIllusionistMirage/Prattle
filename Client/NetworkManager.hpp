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
            bool receive(std::string& message);
            bool receive(std::string& sender, std::string& message);
            bool send(const std::string& sender, const std::string& receiver, const std::string& message);
            void reset();
            void setSocketBlocking(bool blocking);
            bool connectToServer(const std::string serverIP, const int port);

        protected:

        private:
            sf::TcpSocket m_clientSocket;
    };
}

#endif // NETWORK_MANAGER_HPP
