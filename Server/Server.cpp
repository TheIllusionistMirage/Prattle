#include "Server.hpp"
#include <iostream>
namespace chat
{
    Server::Server() :
        timeOut(sf::seconds(60))
    {
        auto status = m_listener.listen(chat::OPEN_PORT);
        if(status != sf::Socket::Done)
        {
            throw std::runtime_error("Fatal error : Error binding the listener at "+std::to_string(chat::OPEN_PORT));
        }
        m_selector.add(m_listener);
        m_running = true;

        std::cout << "          ==========================" << std::endl;
        std::cout << "          |  CHAT PROGRAM - v 0.1  |" << std::endl;
        std::cout << "          ==========================" << std::endl << std::endl;

        std::cout << "By texus, amhndu & TheIllusionistMirage" << std::endl << std::endl;

        std::cout << "--- Server went up at " << chat::getCurrentTimeAndDate() << " ---" << std::endl;
        std::cout << "--- Listening to incoming connections at port " << chat::OPEN_PORT << " ---" << std::endl;
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

    void Server::addNewClient()
    {
        std::unique_ptr<sf::TcpSocket> newClient{new sf::TcpSocket};

        if (m_listener.accept(*newClient) == sf::Socket::Done)
        {
            m_selector.add(*newClient);
            newClient->setBlocking(false);
            newConnections.push_back(std::move(newClient));

///TO KC : Feel free to delete the following commented lines
//            sf::Packet loginPacket;
//            std::string userName;
//            std::string password;
//            std::string info;
//
//            auto status = newClient->receive(loginPacket);
//
//            if (status == sf::Socket::Done)
//            {
//                if (loginPacket >> userName >> password >> info)
//                {
//                    if (isUserRegistered(userName, password) && info == "existing_user")
//                    {
//                        std::string msg = "registered";
//                        sf::Packet msgPacket;
//                        msgPacket << msg;
//
//                        if (newClient->send(msgPacket) != sf::Socket::Done)
//                        {
//                            std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: \nCouln't Send msgPacket to User " << userName << std::endl;
//                            m_selector.remove(*newClient);
//
//                            return false;
//                        }
//
//                        std::cout << "[" + userName + "] joined chat" << std::endl;
//
//                        auto itr_end = m_messages.upper_bound(userName);
//                        for(auto itr = m_messages.lower_bound(userName) ; itr != itr_end ; ++itr)
//                        {
//                            if (newClient->send(itr->second.second) != sf::Socket::Done)
//                            {
//                                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR ::An error occured in sending message from "<< itr->first << " to " << userName << std::endl;
//                            }
//                        }
//
//                        m_messages.erase(userName);
//                        m_clients.insert(std::make_pair(userName, std::move(newClient)));
//
//                        return true;
//                    }
//
//                    if (info == "new_user")
//                    {
//                        if (addNewUser(userName, password))
//                        {
//                            std::string msg = "registered";
//                            sf::Packet msgPacket;
//                            msgPacket << msg;
//
//                            if (newClient->send(msgPacket) != sf::Socket::Done)
//                            {
//                                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: \nCouln't Send msgPacket to User " << userName << std::endl;
//                                m_selector.remove(*newClient);
//
//                                return false;
//                            }
//
//                            m_selector.remove(*newClient);
//                        }
//
//                        else
//                            std::cout << "Couldn't register new user!" << std::endl;
//                    }
//
//                    if (!isUserRegistered(userName, password))
//                    {
//                        std::string msg = "unregistered";
//                        sf::Packet msgPacket;
//                        msgPacket << msg;
//
//                        if (newClient->send(msgPacket) != sf::Socket::Done)
//                        {
//                            std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: \nCouln't Send msgPacket to User " << userName << std::endl;
//                        }
//
//                        m_selector.remove(*newClient);
//
//                        return false;
//                    }
//                }
//            }
//            else if(status == sf::Socket::NotReady)
//            {
//                newConnections.push_back(std::move(newClient));
//            }
//            else
//            {
//                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: Unable to receive data from client!" << std::endl;
//                return false;
//            }
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

    void Server::receive()
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
                    std::cout << "[" + itr->first + "] left chat" << std::endl;
                    m_selector.remove(*itr->second);
                    itr = m_clients.erase(itr);

                    continue;
                }
            }

            itr++;
        }
        for(auto i = newConnections.begin() ; i != newConnections.end() ; )
        {
            if(m_selector.isReady(**i))
            {
                sf::Packet loginPacket;
                std::string userName;
                std::string password;
                std::string info;

                auto status = (*i)->receive(loginPacket);

                if (status == sf::Socket::Done)
                {
                    if (loginPacket >> userName >> password >> info)
                    {
                        if (db.isValidPassword(userName, password) && info == "existing_user")
                        {
                            std::string msg = "registered";
                            sf::Packet msgPacket;
                            msgPacket << msg;

                            if ((*i)->send(msgPacket) != sf::Socket::Done)
                            {
                                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: \nCouln't Send msgPacket to User " << userName << std::endl;
                                m_selector.remove(**i);
                                i = newConnections.erase(i);
                                continue;
                            }

                            std::cout << "[" + userName + "] joined chat" << std::endl;

                            auto itr_end = m_messages.upper_bound(userName);
                            for(auto itr = m_messages.lower_bound(userName) ; itr != itr_end ; ++itr)
                            {
                                if ((*i)->send(itr->second.second) != sf::Socket::Done)
                                {
                                    std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR ::An error occured in sending message from "<< itr->first << " to " << userName << std::endl;
                                }
                            }

                            m_messages.erase(userName);
                            m_clients.insert(std::make_pair(userName, std::move(*i)));
                            i = newConnections.erase(i);
                        }

                        if (info == "new_user")
                        {
                            if (db.addNewUser(userName, password))
                            {
                                std::string msg = "registered";
                                sf::Packet msgPacket;
                                msgPacket << msg;

                                if ((*i)->send(msgPacket) != sf::Socket::Done)
                                {
                                    std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: \nCouln't Send msgPacket to User " << userName << std::endl;
                                }
                                m_selector.remove(**i);
                                i = newConnections.erase(i);
                            }

                            else
                                std::cout << "Couldn't register new user!" << std::endl;
                        }

                        if (! db.isValidPassword(userName, password))
                        {
                            std::string msg = "unregistered";
                            sf::Packet msgPacket;
                            msgPacket << msg;

                            if ((*i)->send(msgPacket) != sf::Socket::Done)
                            {
                                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: \nCouldn't Send msgPacket to User " << userName << std::endl;
                            }

                            m_selector.remove(**i);
                            i = newConnections.erase(i);
                        }
                    }
                    else
                    {
                        std::cerr << __FILE__ << ":" << __LINE__ << "ERROR :: \nLogin packet invalid" << std::endl;
                        ++i;
                    }
                }
                else
                {
                    std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: Unable to receive data from client!" << std::endl;
                    ++i;
                }
            }
            else    ++i;
        }
    }

    void Server::shutdown()
    {
        m_listener.close();
    }
}
