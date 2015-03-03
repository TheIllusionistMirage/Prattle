/**

    Prattle/Client/NetworkManager.cpp
    =================================

    Contains implementations of class NetworkManager declared in Prattle/Client/NetworkManager.hpp

*/

#include "../include/NetworkManager.hpp"

namespace prattle
{
    NetworkManager::NetworkManager() : m_clientSocket{}
    {
    }

    void NetworkManager::reset()
    {
        m_clientSocket.disconnect();
        m_clientSocket.setBlocking(true);
    }

    bool NetworkManager::connectToServer(const std::string serverIP, const int port)
    {
        sf::Socket::Status status = m_clientSocket.connect(serverIP, port);

        if (status == sf::Socket::Done)
            return true;

        return false;
    }

    void NetworkManager::setSocketBlocking(bool blocking)
    {
        m_clientSocket.setBlocking(blocking);
    }

    bool NetworkManager::send(const sf::Packet& packet)
    {
        sf::Packet packetCopy{packet};
        std::string protocol;

        if (packetCopy >> protocol)
        {
            if ( protocol == LOGIN       ||
                  protocol == SIGNUP      ||
                   protocol == SEND_MSG    ||
                    protocol == SEARCH_USER ||
                     protocol == ADD_FRIEND   )
            {
                std::string sender, receiver, msg;
                packetCopy >> sender >> receiver >> msg;
                LOG(protocol + sender + receiver + msg);
                sf::Socket::Status status = m_clientSocket.send(packetCopy);

                if (status == sf::Socket::Status::Done)
                    return true;

                return false;
            }

            else
            {
                LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                return false;
            }
        }

        else
        {
            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
            return false;
        }
    }

    bool NetworkManager::receive(sf::Packet& packet)
    {
        sf::Socket::Status status = m_clientSocket.receive(packet);
        sf::Packet packetCopy{packet};

        if (status == sf::Socket::NotReady)
            return false;

        else if (status == sf::Socket::Done)
        {
            std::string protocol;

            if (packetCopy >> protocol)
            {
                if (protocol == LOGIN_SUCCESS)
                {
                    std::string sender, user, frnd;
                    unsigned short friendCount;
                    std::vector<std::string> friends;

                    if (packetCopy >> sender >> user >> friendCount)
                    {
                        if (sender == SERVER)
                        {
                            for (auto i = 1; i <= 2; ++i)
                            {
                                packetCopy >> frnd;
                                friends.push_back(frnd);
                            }

                            LOG("Client \'" + user + "\' successfully logged in.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == LOGIN_FAILURE)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user >> details)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Server to Client :: " + details);
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SIGNUP_SUCCESS)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Client \'" + user + "\' successfully signed up.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SIGNUP_FAILURE)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user >> details)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Server to Client :: " + details);
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SEND_MSG)
                {
                    std::string sender, source, user, message;

                    if (packetCopy >> sender >> source >> user >> message)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Client \'" + user + "\' successfully received packet.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SEND_MSG_SUCCESS)
                {
                    std::string sender, user;

                    if (packetCopy >> sender >> user)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Client \'" + user + "\' successfully sent packet.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SEND_MSG_FAILURE)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user >> details)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Server to Client :: " + details);
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SEARCH_USER_RESULTS)
                {
                    std::string sender, user, result;

                    if (packetCopy >> sender >> user >> result)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Successfully obtained details from server about user \'" + result + "\'.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == ADD_FRIEND_SUCCESS)
                {
                    std::string sender, user;

                    if (packetCopy >> sender >> user)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Client \'" + user + "\' successfuly added a friend.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == ADD_FRIEND_FAILURE)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user >> details)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Server to Client :: " + details);
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else
                {
                    LOG("ERROR :: An unknown protocol \'" + protocol + "\' is being tried be executed by the client.");
                }
            }
        }

        else if (status == sf::Socket::Error)
            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Unable to receive data from remote peer!" <<  std::endl;

        return false;

    }
}
