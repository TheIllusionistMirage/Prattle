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
            std::string password;
            std::string info;

            auto status = newClient->receive(loginPacket);

            if (status == sf::Socket::Done)
            {
                if (loginPacket >> userName >> password >> info)
                {
                    if (isUserRegistered(userName, password) && info == "existing_user")
                    {
                        std::string msg = "registered";
                        sf::Packet msgPacket;

                        msgPacket << msg;
                        if (newClient->send(msgPacket) != sf::Socket::Done)
                        {
                            std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: \nCouln't Send msgPacket to User " << userName << std::endl;
                            m_selector.remove(*newClient);

                            return false;
                        }

                        std::cout << "[" + userName + "] joined chat! Welcome!" << std::endl;

                        auto itr_end = m_messages.upper_bound(userName);
                        for(auto itr = m_messages.lower_bound(userName) ; itr != itr_end ; ++itr)
                        {
                            if (newClient->send(itr->second.second) != sf::Socket::Done)
                            {
                                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR ::An error occured in sending message from "<< itr->first << " to " << userName << std::endl;
                            }
                        }
                        m_messages.erase(userName);
                        m_clients.insert(std::make_pair(userName, std::move(newClient)));

                        return true;
                    }

                    if (info == "new_user")
                    {
                        if (addNewUser(userName, password))
                        {
                            std::string msg = "registered";
                            sf::Packet msgPacket;

                            msgPacket << msg;
                            if (newClient->send(msgPacket) != sf::Socket::Done)
                            {
                                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: \nCouln't Send msgPacket to User " << userName << std::endl;
                                m_selector.remove(*newClient);

                                return false;
                            }

                             m_selector.remove(*newClient);
                        }

                        else
                            std::cout << "Couldn't register new user!" << std::endl;
                    }

                    if (!isUserRegistered(userName, password))
                    {
                        std::string msg = "unregistered";

                        sf::Packet msgPacket;
                        msgPacket << msg;

                        if (newClient->send(msgPacket) != sf::Socket::Done)
                        {
                            std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: \nCouln't Send msgPacket to User " << userName << std::endl;
                        }
                        m_selector.remove(*newClient);

                        return false;
                    }
                }
            }
            else
            {
                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: Unable to receive data from client!" << std::endl;
            }

            std::cout << userName + "&" + password + "&" + info << std::endl;
        }
    }

    bool Server::send(const std::string &senderUserName, const std::string &receiverUserName, const sf::Packet& dataPacket)
    {
        auto itr = m_clients.find(receiverUserName);
        bool result = true;
        if (itr == m_clients.end())
        {
            m_messages.insert(std::make_pair(receiverUserName,std::make_pair(senderUserName, dataPacket)));
        }

        else
        {
            sf::Packet msgPacket{dataPacket};

            if (itr->second->send(msgPacket) != sf::Socket::Done)
            {
                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR ::An error occured in sending message from "<< senderUserName << " to " << receiverUserName << std::endl;
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
                std::string sender;
                std::string receiver;
                std::string msg;

                auto status = (itr->second)->receive(dataPacket);

                if (status == sf::Socket::Done)
                {
                    if (dataPacket >> sender >> receiver >> msg)
                    {
                        sf::Packet msgPacket;
                        msgPacket << sender << msg;

                        if (!send(sender, receiver, msgPacket))
                            std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: Error in sending message to user!" << std::endl;
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

    bool Server::isUserRegistered(const std::string& userName, const std::string& password)
    {
        if (m_userDatabase.is_open())
            m_userDatabase.close();

        m_userDatabase.open(USER_LIST, std::ios::in | std::ios::out | std::ios::app);

        if (m_userDatabase.is_open() && m_userDatabase.good())
        {
            std::vector<std::string> parsedRecords = getRecords();
            auto itr = std::find(parsedRecords.begin(), parsedRecords.end(), userName + ":" + password);

            if (itr != parsedRecords.end())
                return true;

            else
                return false;
        }
    }

    bool Server::addNewUser(const std::string& userName, const std::string& password)
    {
        if (!isUserRegistered(userName, password))
        {
            if (m_userDatabase.is_open())
                m_userDatabase.close();

            m_userDatabase.open(USER_LIST, std::ios::in | std::ios::out | std::ios::app);

            if (m_userDatabase.is_open() && m_userDatabase.good())
            {
                m_userDatabase.seekp(std::ios_base::end);
                m_userDatabase << userName << ":" << password << std::endl;

                return true;
            }
        }

        else
        {
            std::cout << "Sorry, but that username's already been taken! Please try another name!" << std::endl;
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

        else
        {
            std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: Error in opening user database!" << std::endl;
        }

        return parsedRecords;
    }

    void Server::shutdown()
    {
        m_listener.close();
        m_userDatabase.close();
    }
}
