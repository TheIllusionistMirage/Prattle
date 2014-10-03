#include "Server.hpp"
#include <iostream>
namespace chat
{
    Server::Server() :
        m_userDatabase(USER_LIST, std::ios::in | std::ios::out | std::ios::app)
    {
        m_server.listen(chat::OPEN_PORT);
        m_selector.add(m_server);
        m_running = true;

        std::cout << "          ==========================" << std::endl;
        std::cout << "          |  CHAT PROGRAM - v 0.1  |" << std::endl;
        std::cout << "          ==========================" << std::endl << std::endl;

        std::cout << "By texus, amhndu & TheIllusionistMirage" << std::endl << std::endl;

        std::cout << "--- Server went up at " << chat::getCurrentTimeAndDate() << " ---" << std::endl;
        std::cout << "--- Listening to incoming connections at port " << chat::OPEN_PORT << " ---" << std::endl;

        if (!m_userDatabase.good())
        {
            throw std::runtime_error("Unable to open the user database " + USER_LIST + " !");
        }
    }

    bool Server::isRunning()
    {
        return m_running;
    }

    bool Server::wait()
    {
        return m_selector.wait();
    }

    bool Server::isReady()
    {
        return m_selector.isReady(m_server);
    }

    bool Server::addNewClient()
    {
        std::unique_ptr<sf::TcpSocket> newClient{new sf::TcpSocket};

        if (m_server.accept(*newClient) == sf::Socket::Done)
        {
            m_selector.add(*newClient);
            sf::Packet loginPacket;
            std::string userName;

            auto status = newClient->receive(loginPacket);

            if (status == sf::Socket::Done)
            {
                if (loginPacket >> userName)
                {
                    if (isUsernameTaken(userName))
                    {
                        std::cout << "[" + userName + "] joined chat! Welcome!" << std::endl;

                        std::string msg = "registered";
                        sf::Packet msgPacket;

                        msgPacket << msg;

                        if (newClient->send(msgPacket) != sf::Socket::Done ||
                            newClient->send(msgPacket) == sf::Socket::Error)
                        {
                            std::cout << "ERROR :: An error occured in logging in! Please try again" << std::endl;
                        }

                        m_clients.insert(std::make_pair(userName, std::move(newClient)));
                    }

                    else
                    {
                        sf::TcpSocket remotePeer;
                        sf::Socket::Status status = remotePeer.connect(remotePeer.getRemoteAddress(), remotePeer.getRemotePort());

                        if (status == sf::Socket::Done)
                        {
                            std::string msg = "unregistered";

                            sf::Packet msgPacket;
                            msgPacket << msg;

                            if (remotePeer.send(msgPacket) != sf::Socket::Done ||
                                remotePeer.send(msgPacket) == sf::Socket::Error)
                            {
                                std::cout << "ERROR :: An error occured in logging in! Please try again" << std::endl;
                            }
                        }

                        m_selector.remove(*newClient);
                        std::cout << "You are not registered with us! Please register to start chatting!" << std::endl;
                    }
                }
            }
            else
            {
                std::cout << "Unable to receive data from client!" << std::endl;
            }
        }
    }

    bool Server::receive()
    {
        for (auto itr = m_clients.begin(); itr != m_clients.end(); )
        {
            if (m_selector.isReady(*itr->second))
            {
                sf::Packet dataPacket;
                std::string data;

                auto status = (itr->second)->receive(dataPacket);

                if (status == sf::Socket::Done)
                {
                    if (dataPacket >> data)
                        std::cout << data << std::endl;
                }

                if (status == sf::Socket::Disconnected)
                {
                    std::cout << "[" + itr->first + "] left chat! See him soon!" << std::endl;
                    m_selector.remove(*itr->second);
                    itr = m_clients.erase(itr);

                    continue;
                }
            }
            itr++;
        }
    }

    void Server::openDatabase(const std::string& userList)
    {
        if (m_userDatabase.is_open())
            m_userDatabase.close();

        m_userDatabase.open(userList, std::ios::in | std::ios::out | std::ios::app);

        if (!m_userDatabase.good())
        {
            throw std::runtime_error("Unable to open the user database " + userList + " !");
        }
    }

    bool Server::isUsernameTaken(const std::string& userName)
    {
        if (m_userDatabase.is_open() && m_userDatabase.good())
        {
            /*std::vector<std::string> parsedRecords;
            m_userDatabase.seekg(0);
            std::string record;

            while (!m_userDatabase.eof())
            {
                std::getline(m_userDatabase, record, '\n');
                parsedRecords.push_back(record);
            }*/

            std::vector<std::string> parsedRecords = getRecords();
            auto itr = std::find(parsedRecords.begin(), parsedRecords.end(), userName);

            if (itr != parsedRecords.end())
                return true;
            else
                return false;
        }
    }

    bool Server::addNewUser(const std::string& userName)
    {
        if (m_userDatabase.is_open() && m_userDatabase.good())
        {
            m_userDatabase.seekp(std::ios_base::end);
            m_userDatabase << userName;

            return true;
        }

        return false;
    }

    std::vector<std::string> Server::getRecords()
    {
        std::vector<std::string> parsedRecords;
        if (m_userDatabase.is_open() && m_userDatabase.good())
        {
            m_userDatabase.seekg(0);

            std::string record;
            while (!m_userDatabase.eof())
            {
                std::getline(m_userDatabase, record, '\n');
                if (record[0] != '#')
                    parsedRecords.push_back(record);
            }
        }

        return parsedRecords;
    }

    void Server::shutdown()
    {
        m_server.close();
        m_userDatabase.close();
    }
}
