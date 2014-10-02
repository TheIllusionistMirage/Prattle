#include "Client.hpp"

namespace chat
{
    Client::Client()
    {
        //m_client.setBlocking(true);
        m_loginStatus = false;
    }

    void Client::loginPrompt()
    {
        char loginOption;

        std::cout << "          ==========================" << std::endl;
        std::cout << "          |  CHAT PROGRAM - v 0.1  |" << std::endl;
        std::cout << "          ==========================" << std::endl << std::endl;

        std::cout << "(C) Copyright texus & The Illusionist Mirage" << std::endl << std::endl;

    }

    void Client::signup()
    {
        /*std::fstream userDatabase(USER_LIST, std::ios::in | std::ios::out);
        std::string newUserName;
        bool success = true;;

        std::cout << "Welcome to Chat Program aignup page! Please fill out the following details." <<std::endl << std::endl;
        std::cout << "Your Username : ";

        do
        {
            success = true;

            std::cout << "Enter a new username : ";
            std::cin.ignore();
            std::getline(std::cin, newUserName);

            std::vector<std::string> existingUser;

            userDatabase.seekg(0);
            while (userDatabase.is_open())
            {
                std::string str;

                char ch;

                while (userDatabase.get(ch))
                {
                    str += ch;
                }

                existingUser.push_back(str);
            }

            for (auto itr = existingUser.begin(); itr != existingUser.end(); itr++)
            {
                if (newUserName == *itr)
                {
                    std::cout << "Sorry, but that username has already been taken! Please use another one.";
                    success = false;
                }
            }
        } while (!success);*/
    }

    //bool Client::login(const std::string& username, const std::string& password)
    bool Client::login()
    {
        sf::Socket::Status loginSuccess = m_client.connect(chat::SERVER_IP_ADDRESS, chat::OPEN_PORT);

        if (loginSuccess == sf::Socket::Done)
        {
            //std::cout << "(L)ogin if you're an existing user OR (R)egister with us today!" << std::endl;
            std::cout << "Enter your username : ";

            std::string userName, serverReply;
            std::getline(std::cin, userName);

            sf::Packet msgPacket;
            msgPacket << userName;

            if (m_client.send(msgPacket) == sf::Socket::Done)
            {
                sf::Socket::Status status = m_client.receive(msgPacket);

                if (status == sf::Socket::Done)
                {
                    if (msgPacket >> serverReply)
                    {
                        if (serverReply != "unregistered")
                        {
                            m_loginStatus = true;
                            std::cout << "Login Successful!" << std::endl;
                        }
                    }
                }
            }

            else if (m_client.send(msgPacket) == sf::Socket::Error)
                std::cout << "ERROR :: An error occured in logging in! Please try again" << std::endl;
        }
        else
        {
            std::cout << "ERROR :: Couldn't connect to the server! Please make sure that the server is up and running." << std::endl;
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
}
