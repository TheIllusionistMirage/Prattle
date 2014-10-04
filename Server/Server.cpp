#include "Server.hpp"
#include <iostream>
namespace chat
{
    Server::Server() :
        m_userDatabase(USER_LIST, std::ios::in | std::ios::out | std::ios::app),
        timeOut(sf::seconds(5))
    {
        m_listener.listen(chat::OPEN_PORT);
        m_selector.add(m_listener);
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
        return m_selector.wait(timeOut);
    }

    bool Server::newConnectionRequest()
    {
        return m_selector.isReady(m_listener);
    }

    bool Server::addNewClient()
    {
        std::unique_ptr<sf::TcpSocket> newClient{new sf::TcpSocket};

        if (m_listener.accept(*newClient) == sf::Socket::Done)
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
                            std::cerr << "ERROR :: Server.cpp:71\tUser: "<< userName << "\nCouldn't Send msgPacket" << std::endl;
                        }

                        m_clients.insert(std::make_pair(userName, std::move(newClient)));
                    }

                    else
                    {
                        std::string msg = "unregistered";

                        sf::Packet msgPacket;
                        msgPacket << msg;

                        if (newClient->send(msgPacket) != sf::Socket::Done ||
                            newClient->send(msgPacket) == sf::Socket::Error)
                        {
                            std::cerr << "ERROR :: Server.cpp:87\tUser: "<< userName << "\nCouln't Send msgPacket" << std::endl;
                        }
                        m_selector.remove(*newClient);
//What's the point of this ? --> std::cout << "You are not registered with us! Please register to start chatting!" << std::endl;
                    }
                }
            }
            else
            {
                std::cerr << "Server.cpp:96 :: Unable to receive data from client!" << std::endl;
            }
        }
    }

    bool Server::send(const std::string &senderUserName, const std::string &receiverUserName, const sf::Packet& dataPacket)
    {
        auto itr = m_clients.find(receiverUserName);
        bool result = true;
        if (itr == m_clients.end())
        {
            m_messages.insert(std::make_pair(std::make_pair(senderUserName, receiverUserName), dataPacket));
        }

        else
        {
            std::string data;
            sf::Packet msgPacket;

            msgPacket = dataPacket;

            if (itr->second->send(msgPacket) != sf::Socket::Done ||
                itr->second->send(msgPacket) == sf::Socket::Error)
            {
                std::cerr << "ERROR :: Server.cpp:120 ::An error occured in sending message from "<< senderUserName << " to " << receiverUserName << std::endl;
                result = false;
            }
        }
        return result;
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
                    {
                        std::string sender;
                        std::string receiver;
                        std::string msg;

                        /*for (auto I = data[0]; I != ':'; I++)
                            user += data[I];

                        std::size_t pos = data.find(":");
                        msg = data.substr(pos);

                        //std::cout << data << std::endl;

                        m_messages.insert(std::make_pair(user, msg));
                        send(user, )*/

                        std::size_t pos = data.find(":");
                        sender = data.substr(0, pos);
                        data = data.substr(pos + 1);
                        receiver = data.substr(0, data.find(":"));
                        msg = data.substr(data.find(":") + 1);

                        sf::Packet msgPacket;
                        msgPacket << msg;

                        //std::cout << sender +  " " + receiver + " " + msg;
                        if (!send(sender, receiver, msgPacket))
                            std::cout << "ERROR :: Error in sending message to user!" << std::endl;
                    }
                }
                else if (status == sf::Socket::Disconnected)
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
        if (m_userDatabase.is_open())
            m_userDatabase.close();

        m_userDatabase.open(USER_LIST, std::ios::in | std::ios::out | std::ios::app);

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

    bool Server::addNewUser()
    {
        std::unique_ptr<sf::TcpSocket> newClient{new sf::TcpSocket};

        if (m_listener.accept(*newClient) == sf::Socket::Done)
        {
            m_selector.add(*newClient);
            sf::Packet loginPacket;
            std::string userName;

            auto status = newClient->receive(loginPacket);

            if (status == sf::Socket::Done)
            {
                if (loginPacket >> userName)
                {
                    if (!isUsernameTaken(userName))
                    {
                        if (m_userDatabase.is_open())
                            m_userDatabase.close();

                        m_userDatabase.open(USER_LIST, std::ios::in | std::ios::out | std::ios::app);

                        if (m_userDatabase.is_open() && m_userDatabase.good())
                        {
                            m_userDatabase.seekp(std::ios_base::end);
                            m_userDatabase << userName;

                            return true;
                        }
                    }
                    else
                    {
                        m_selector.remove(*newClient);
                        std::cout << "Sorry, but that username's already been taken!" << std::endl;
                    }
                }
                std::cout << "ERROR :: Unable to receive data from remote client!" << std::endl;
            }
        }

        return false;
    }

    std::vector<std::string> Server::getRecords()
    {
        std::vector<std::string> parsedRecords;
        if (m_userDatabase.is_open() && m_userDatabase.good())
        {
            //m_userDatabase.seekg(0);

            std::string record;
            while (!m_userDatabase.eof())
            {
                std::getline(m_userDatabase, record, '\n');
                if (record[0] != '#')
                    parsedRecords.push_back(record);
            }
        }
        else
        {
            std::cout << "ERROR :: Error in opening user database!" << std::endl;
        }

        return parsedRecords;
    }

    void Server::shutdown()
    {
        m_listener.close();
        m_userDatabase.close();
    }
}
