#include "Client.hpp"

namespace chat
{
    Client::Client() :
        m_loginStatus(false),
        m_onlineStatus(status::Unavailable)
    {
    }

    void Client::loginPrompt()
    {
        char loginOption;

        std::cout << "          ==========================" << std::endl;
        std::cout << "          |  CHAT PROGRAM - v 0.1  |" << std::endl;
        std::cout << "          ==========================" << std::endl << std::endl;

        std::cout << "By texus, amhndu & TheIllusionistMirage" << std::endl << std::endl;

    }

    void Client::signup()
    {
        /*std::fstream userDatabase(USER_LIST, std::ios::in | std::ios::out | std::ios::app);       std::string newUserName;
        bool success = true;;

        std::cout << "Welcome to Chat Program signup page! Please fill out the following details." <<std::endl << std::endl;
        std::cout << "Your Username : ";
        std::getline(std::cin, newUserName);

        userDatabase.seekg(0);

        if (userDatabase.good() && userDatabase.is_open())
        {
            while (!userDatabase.eof())
            {
                std::string record;
                std::getline(userDatabase, record, '\n');

                if (record[0] != '#')
                    continue;
                else
                    if (record == newUserName)
                        success = false;
            }

            if (!success)
                std::cout << "The username \"" + newUserName + "\" is already been taken! Please use something else." << std::endl;
            else
            {
                userDatabase.seekp(std::ios::end);
                userDatabase << newUserName;
            }
        }*/
    }

    bool Client::login()
    {
        sf::Socket::Status loginSuccess = m_client.connect(chat::SERVER_IP_ADDRESS, chat::OPEN_PORT);

        if (loginSuccess == sf::Socket::Done)
        {
            std::cout << "Enter your username : ";

            std::string serverReply;
            std::getline(std::cin, m_userName);

            sf::Packet msgPacket;
            msgPacket << m_userName;

            if (m_client.send(msgPacket) == sf::Socket::Done)
            {
                sf::Socket::Status status = m_client.receive(msgPacket);

                if (status == sf::Socket::Done)
                {
                    if (msgPacket >> serverReply)
                    {
                        if (serverReply == "registered")
                        {
                            m_loginStatus = true;
                            std::cout << "Login Successful!" << std::endl;

                            std::cout << "Enter the name of the person you want to chat with : ";
                            std::getline(std::cin, m_friend);
                            m_onlineStatus = Client::status::Available;
                            m_client.setBlocking(false);
                        }
                        else if(serverReply == "unregistered")
                        {
                            std::cout << "You are not registered with us! Please register to start chatting!" << std::endl;
                        }
                        else
                        {
                            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Unknown reply from the server" << std::endl;
                        }
                    }
                }
            }
            else if (m_client.send(msgPacket) == sf::Socket::Error)
                std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: An error occured in logging in! Please try again" << std::endl;
        }
        else
        {
            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Couldn't connect to the server! Please make sure that the server is up and running." << std::endl;
        }

    }

    bool Client::receive()
    {
        sf::Packet dataPacket;
        sf::Socket::Status status = m_client.receive(dataPacket);

        while(status == sf::Socket::Done)
        {
            std::string data;
            if (dataPacket >> data)
            {
                std::cout << data << std::endl;
            }
            status = m_client.receive(dataPacket);
        }
        if (status == sf::Socket::Error)
        {
            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Unable to receive data from remote peer!" <<  std::endl;
        }
    }

    bool Client::logout()
    {
        m_client.disconnect();
        m_loginStatus = false;
    }

    bool Client::isLoggedIn()
    {
        return m_loginStatus;
    }

    sf::Socket::Status Client::send(sf::Packet& packet)
    {
        return m_client.send(packet);
    }

    const std::string& Client::getUserName()
    {
        return m_userName;
    }

    const std::string& Client::getFriendName()
    {
        return m_friend;
    }
}
