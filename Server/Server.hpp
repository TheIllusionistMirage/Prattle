/*

The server which hosts the chat process.

For clients to start chatting, they need to connect to the server.

*/

#ifndef SERVER_HPP
#define SERVER_HPP


#include <iostream>
#include <fstream>
#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <list>
#include <SFML/Network.hpp>
#include "System.hpp"
#include "UserDatabase.h"

namespace chat
{
    class Server
    {
        public:
            Server();

            bool isRunning();

            bool wait();

            bool newConnectionRequest();

            void addNewClient();

            bool send(const std::string &senderUserName,
                      const std::string &receiverUserName,
                      const sf::Packet& dataPacket);

            void receive();

            void shutdown();

        private:
            sf::TcpListener m_listener;
            sf::SocketSelector m_selector;
            std::map<std::string, std::unique_ptr<sf::TcpSocket>> m_clients;
            bool m_running;
            UserDatabase db;
            sf::Time timeOut;
            std::multimap<std::string, std::pair<std::string,sf::Packet>> m_messages;
            std::list<std::unique_ptr<sf::TcpSocket>> newConnections;
    };
}

#endif // SERVER_HPP
