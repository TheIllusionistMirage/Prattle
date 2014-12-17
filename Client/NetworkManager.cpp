#include "NetworkManager.hpp"

namespace prattle
{
    NetworkManager::NetworkManager() : m_clientSocket{}
    {
        m_loginStatus = false;
        m_onlineStatus = Status::Offline;
    }

    void NetworkManager::reset()
    {
        m_loginStatus = false;
        m_onlineStatus = Status::Offline;
    }

    bool NetworkManager::checkIfWhitespace(const std::string& message)
    {
        for (auto itr = message.begin(); itr != message.end(); itr++)
        {
            if (*itr != ' ' && *itr != '\t' && *itr != '\n')
            {
                return false;
            }
        }
        return true;
    }

    //void CoreEngine::run(const sf::RenderWindow* sf_window)

    void NetworkManager::signup(const std::string& username, const std::string& password)
    {
        sf::Socket::Status connectionSuccess = m_clientSocket.connect(SERVER_IP_ADDRESS, OPEN_PORT);

        if (connectionSuccess == sf::Socket::Done)
        {
            std::string info = "new_user";

            sf::Packet packet;
            packet << username << password << info;

            if (m_clientSocket.send(packet) == sf::Socket::Done)
            {
                sf::Socket::Status status = m_clientSocket.receive(packet);

                if (status == sf::Socket::Done)
                {
                    std::string serverReply;

                    if (packet >> serverReply)
                    {
                        if (serverReply == "registered")
                        {
                            m_loginStatus = false;
                            //std::cout << "Registration successful!" << std::endl;
                            //std::cout << "Now login to start chatting!" << std::endl;
                        }

                        else if (serverReply != "registered")
                        {
                            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Problem occured in registration! Please try again" << std::endl;
                        }
                    }
                }
            }
            else if (m_clientSocket.send(packet) == sf::Socket::Error)
                std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: An error occured in registration! Please try again" << std::endl;
        }
        else
        {
            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: An error occurred in establishing connection with the server! Please try again." << std::endl;
        }
    }

    bool NetworkManager::login(const std::string& username, const std::string& password)
    {
        sf::Socket::Status connectionSuccess = m_clientSocket.connect(prattle::SERVER_IP_ADDRESS, prattle::OPEN_PORT);

        if (connectionSuccess == sf::Socket::Done)
        {
            std::string info = "existing_user";

            if (username != "" && password != "")
            {
                sf::Packet packet;
                packet << username << password << info;

                if (m_clientSocket.send(packet) == sf::Socket::Done)
                {
                    sf::Socket::Status status = m_clientSocket.receive(packet);

                    if (status == sf::Socket::Done)
                    {
                        std::string serverReply;

                        if (packet >> serverReply)
                        {
                            if (serverReply == "registered")
                            {
                                m_loginStatus = true;
                                m_onlineStatus = Status::Online;

                                std::cout << "Enter the name of the person you want to chat with : ";
                                std::string name;
                                std::getline(std::cin, name);
                                m_friends.empty();
                                m_friends.push_back(name);
                            }

                            else if(serverReply == "unregistered")
                            {
                                std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Unknown username/password combination!" << std::endl;
                            }

                            else
                            {
                                std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Unknown reply from the server" << std::endl;
                            }
                        }
                    }
                }

                else if (m_clientSocket.send(packet) == sf::Socket::Error)
                    std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: An error occured in logging in! Please try again" << std::endl;
            }

            else
            {
                std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Either username or password field is blank!" << std::endl;
            }
        }

        else
        {
            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Can't connect to server! Please try again" << std::endl;
        }
    }

    bool NetworkManager::isLoggedIn()
    {
        return m_loginStatus;
    }

    sf::Socket::Status NetworkManager::send(sf::Packet& packet)
    {
        return m_clientSocket.send(packet);
    }

    std::string NetworkManager::receive()
    {
        sf::Packet packet;
        sf::Socket::Status status = m_clientSocket.receive(packet);

        while(status == sf::Socket::Done)
        {
            std::string sender;
            std::string data;

            if (packet >> sender >> data)
            {
                if (data != "")
                    return data;
            }

            status = m_clientSocket.receive(packet);
        }

        if (status == sf::Socket::Error)
        {
            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Unable to receive data from remote peer!" <<  std::endl;
            return "";
        }

        return "";
    }

    std::string const& NetworkManager::getUserName() const
    {
        return m_userName;
    }

    std::vector<std::string> const& NetworkManager::getFriendsName() const
    {
        return m_friends;
    }

    bool NetworkManager::logout()
    {
        m_clientSocket.disconnect();
        m_clientSocket.setBlocking(true);
        reset();
    }
}
