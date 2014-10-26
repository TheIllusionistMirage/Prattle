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
#include <SFML/Network.hpp>
#include "System.hpp"

namespace chat
{
    class Server
    {
        public:
            Server();

            bool isRunning();

            bool wait();

            bool newConnectionRequest();

            bool addNewClient();

            bool send(const std::string &senderUserName,
                      const std::string &receiverUserName,
                      const sf::Packet& dataPacket);

            void receive();

            void openDatabase(const std::string& userList = chat::USER_LIST);

            bool isUserRegistered(const std::string& userName,
                                  const std::string& password);

            bool addNewUser(const std::string& userName,
                            const std::string& password);

            std::vector<std::string> getRecords();

            void shutdown();

        protected:
            sf::Time timeOut;

        private:
            sf::TcpListener m_listener;
            sf::SocketSelector m_selector;
            std::map<std::string, std::unique_ptr<sf::TcpSocket>> m_clients;
            bool m_running;
            std::fstream m_userDatabase;
            std::multimap<std::string, std::pair<std::string,sf::Packet>> m_messages;
    };
}

#endif // SERVER_HPP
