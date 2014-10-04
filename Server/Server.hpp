/*

The server which hosts the chat process.

For clients to start chatting, they need to connect to the server.

*/

#ifndef SERVER_HPP
#define SERVER_HPP


#include <ctime>
#include <map>
#include <fstream>
#include <exception>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <memory>
#include "System.hpp"

namespace chat
{
    class Server
    {
        public:
            Server();

            bool isRunning();
            bool wait();
            bool isReady();
            bool addNewClient();
            bool receive();
            bool send(const std::string &senderUserName, const std::string &receiverUserName, const sf::Packet& dataPacket);
            void openDatabase(const std::string& userList = chat::USER_LIST);
            void shutdown();
            std::vector<std::string> getRecords();
            bool isUsernameTaken(const std::string& userName);
            bool addNewUser();

        protected:
            sf::Time timeOut;

        private:
            sf::TcpListener m_server;
            std::map<std::string, std::unique_ptr<sf::TcpSocket>> m_clients;
            sf::SocketSelector m_selector;
            bool m_running;
            std::vector<std::string> m_members;
            std::fstream m_userDatabase;
            std::map<std::pair<std::string, std::string>, sf::Packet> m_messages;
    };
}

#endif // SERVER_HPP
