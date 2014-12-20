#include "Client.hpp"

namespace prattle
{
    Client::Client() : m_networkManager{},
                       m_ui{},
                       m_loginStatus{false},
                       m_onlineStatus{Status::Offline},
                       m_username{""},
                       m_password{""}
    {
        m_windowPtr = m_ui.getRenderWindow();
        m_guiPtr = m_ui.getGui();
        m_networkManager.reset();
        m_ui.reset();
        //m_ui.m_loginButton->connect("pressed", &Client::login, this, m_username, m_password);
        m_ui.m_loginButton->connect("pressed", &Client::login, this);
    }

    void Client::reset()
    {
        m_loginStatus = false;
        m_onlineStatus = Status::Offline;
        m_username = "";
        m_password = "";

        m_networkManager.reset();
        m_ui.reset();
    }

    void Client::run()
    {
        while (m_windowPtr->isOpen())
        {
            sf::Event event;
            while (m_windowPtr->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    m_windowPtr->close();

                if (event.type == sf::Event::Resized)
                {
                    m_windowPtr->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                    m_guiPtr->setView(m_windowPtr->getView());
                }

                if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
                {
                    if (isLoggedIn())
                    {
                        if (!checkIfWhitespace(m_ui.getInputText()))
                        {
                            std::string message = m_ui.getInputText();

                            if (!m_networkManager.send(m_username, m_friend, message))
                            {
                                std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Error in sending message! Please try again" << std::endl;
                            }

                            else
                            {
                                m_ui.addTextToChatBox(m_username, message);
                                m_ui.clearInputTextBox();
                            }
                        }
                    }
                }

                m_guiPtr->handleEvent(event);
            }

            if (isLoggedIn())
            {
                std::string sender;
                std::string message;

                while (m_networkManager.receive(sender, message))
                {
                    m_ui.addTextToChatBox(sender, message);
                }
            }

            m_username = m_ui.getUsernameFieldText();
            m_password = m_ui.getPasswordFieldText();

            m_windowPtr->clear(sf::Color::Black);
            m_guiPtr->draw();
            m_windowPtr->display();
        }
    }

    bool Client::isLoggedIn()
    {
        return m_loginStatus;
    }

    //bool Client::login(const std::string& username, const std::string& password)
    bool Client::login()
    {
        /*sf::Socket::Status connectionSuccess = m_networkManager.m_clientSocket.connect(prattle::SERVER_IP_ADDRESS, prattle::OPEN_PORT);

        if (connectionSuccess == sf::Socket::Done)
        {
            std::string info = "existing_user";

            if (m_username != "" && m_password != "")
            {
                sf::Packet packet;
                packet << m_username << m_password << info;

                if (m_networkManager.m_clientSocket.send(packet) == sf::Socket::Done)
                {
                    sf::Socket::Status status = m_networkManager.m_clientSocket.receive(packet);

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
                                //m_friends.empty();
                                //m_friends.push_back(name);
                                m_friend = name;
                                m_networkManager.setSocketBlocking(false);
                                m_ui.insertNewFriendTab(name);
                                m_ui.changeScreenState(ScreenState::ChatScreen);
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

                else if (m_networkManager.m_clientSocket.send(packet) == sf::Socket::Error)
                {
                    std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: An error occured in logging in! Please try again" << std::endl;
                    m_networkManager.reset();
                }
            }

            else
            {
                std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Either username or password field is blank!" << std::endl;
                m_networkManager.reset();
            }
        }

        else
        {
            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Can't connect to server! Please try again" << std::endl;
            m_networkManager.reset();
        }*/

        if (m_networkManager.connectToServer(SERVER_IP_ADDRESS, OPEN_PORT))
        {
            if (m_username != "" && m_password != "")
            {
                if (m_networkManager.send(m_username, m_password, "existing_user"))
                {
                    std::string serverReply;
                    m_networkManager.receive(serverReply);

                    if (!serverReply.empty())
                    {
                        // In this case, the client socket receives only
                        // one string "registered" or "unregistered" as the reply.

                        if (serverReply == "registered")
                        {
                            m_loginStatus = true;
                            m_onlineStatus = Status::Online;

                            std::cout << "Enter the name of the person you want to chat with : ";
                            std::string name;
                            std::getline(std::cin, name);
                            m_friend = name;
                            m_networkManager.setSocketBlocking(false);
                            m_ui.insertNewFriendTab(name);
                            m_ui.changeScreenState(ScreenState::ChatScreen);
                        }

                        else if (serverReply == "unregistered")
                        {
                            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: You are not registered with us! Please register before logging in!" << std::endl;
                            m_networkManager.reset();

                            return false;
                        }
                    }
                }

                else
                {
                    std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to send login info to server!" << std::endl;
                    m_networkManager.reset();

                    return false;
                }
            }
        }

        else
        {
            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Can't connect to server! Please try again" << std::endl;
            m_networkManager.reset();

            return false;
        }
    }

    void Client::signup(const std::string& username, const std::string& password)
    {
        /*sf::Socket::Status connectionSuccess = m_clientSocket.connect(SERVER_IP_ADDRESS, OPEN_PORT);

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
        }*/
    }

    bool Client::checkIfWhitespace(const std::string& message)
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

    std::string const& Client::getUserName() const
    {
        return m_username;
    }

    std::string const& Client::getFriendsName() const
    {
        return m_friend;
    }

    bool Client::logout()
    {
        reset();
    }
}
