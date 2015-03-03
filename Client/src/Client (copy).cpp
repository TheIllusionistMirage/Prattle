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
        //m_windowPtr = m_ui.getRenderWindow();
        //m_guiPtr = m_ui.getGui();
        m_networkManager.reset();
        m_ui.reset();

        m_ui.m_loginButton->connect("pressed", &Client::login, this);
        m_ui.m_submitButton->connect("pressed", &Client::signup, this);
        m_ui.m_logoutButton->connect("pressed", &Client::logout, this);
        m_ui.m_searchButton->connect("pressed", &Client::searchUsername, this);
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
        //while (m_windowPtr->isOpen())
        while (m_ui.getRenderWindow()->isOpen())
        {
            //m_ui.getRenderWindow()->close();
            std::cout << "Z";
            sf::Event event;
            //while (m_windowPtr->pollEvent(event))
            while (m_ui.getRenderWindow()->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    //m_windowPtr->close();
                    m_ui.getRenderWindow()->close();

                if (event.type == sf::Event::Resized)
                {
                    //m_windowPtr->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                    m_ui.getRenderWindow()->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                    //m_guiPtr->setView(m_windowPtr->getView());
                    m_ui.getGui()->setView(m_ui.getRenderWindow()->getView());
                }

                if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
                {
                    if (isLoggedIn())
                    {
                        if (!checkIfWhitespace(m_ui.getInputText()))
                        {
                            std::string message = m_ui.getInputText();

                            sf::Packet packet;
                            packet << SEND_MSG << m_username << m_friend << message;

                            if (!m_networkManager.send(packet))
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

                //m_guiPtr->handleEvent(event);
                m_ui.getGui()->handleEvent(event);
            }

            if (isLoggedIn())
            {
                std::string protocol, sender, source, receiver, message;

                sf::Packet packet;
                while (m_networkManager.receive(packet))
                {
                    packet >> protocol >> source >> sender >> receiver >> message;
                    m_ui.addTextToChatBox(sender, message);
                }
            }

            m_ui.updateWidgets();

            //std::string a = m_ui.m_searchResults->getSelectedItem().toAnsiString();
            //std::cout << a << std::endl;

            m_username = m_ui.getUsernameFieldText();
            m_password = m_ui.getPasswordFieldText();

            //m_windowPtr->clear(DEFAULT_BG_COLOR);
            //m_windowPtr->clear(sf::Color::Black);
            m_ui.getRenderWindow()->clear(sf::Color::Black);
            m_ui.getGui()->draw();
            m_ui.getRenderWindow()->display();
        }
    }

    bool Client::isLoggedIn()
    {
        return m_loginStatus;
    }

    //bool Client::login(const std::string& username, const std::string& password)
    bool Client::login()
    {
        if (m_networkManager.connectToServer(SERVER_IP_ADDRESS, OPEN_PORT))
        {
            //if (m_username != "" || m_password != "")
            if (!checkIfWhitespace(m_username) && !checkIfWhitespace(m_password))
            {
                sf::Packet packet;
                packet << LOGIN << m_username << SERVER << m_password ;

                if (m_networkManager.send(packet))
                {
                    sf::Packet replyPacket;
                    m_networkManager.receive(replyPacket);

                    std::string protocol, sender, user, frnd;
                    unsigned int friendCount;
                    std::vector<std::string> friends;

                    if (replyPacket >> protocol >> sender >> user >> friendCount)
                    {
                        if (protocol == LOGIN_SUCCESS)
                        {
                            //std::cout << protocol << " " << sender << " " << user << " " << friendCount;
                            for (auto i = 1; i <= friendCount; ++i)
                            {
                                replyPacket >> frnd;
                                //std::cout << " " << frnd;
                                friends.push_back(frnd);
                            }

                            m_friends = friends;
                            //std::cout << friendCount << std::endl;

                            m_loginStatus = true;
                            m_onlineStatus = Status::Online;

                            m_networkManager.setSocketBlocking(false);
                            m_ui.setChatUsername(m_username);
                            m_ui.initFriendList(m_friends);
                            m_ui.changeScreenState(ScreenState::ChatScreen);

                            return true;
                        }

                        else if (protocol == LOGIN_FAILURE)
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Error!");
                            if (!m_ui.m_messageWindow->isVisible())
                                //m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                            m_ui.m_messageLabel->setText("Can't recognize the username-password combination!\nIf you're not registered with us, please consider signing up.");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: You are not registered with us! Please register before logging in!" << std::endl;
                            m_networkManager.reset();

                            return false;
                        }
                    }

                    else
                    {
                        m_ui.m_messageWindow->setSize(400, 60);
                        m_ui.m_messageWindow->setTitle("Error!");
                        if (!m_ui.m_messageWindow->isVisible())
                            m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                        m_ui.m_messageLabel->setText("Unkown reply from server. Please try again later");
                        m_ui.m_messageLabel->setTextSize(12);
                        m_ui.m_messageLabel->setPosition(20, 10);

                        if (!m_ui.m_messageWindow->isVisible())
                            m_ui.m_messageWindow->show();

                        std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: You are not registered with us! Please register before logging in!" << std::endl;
                        m_networkManager.reset();

                        return false;
                    }

                    /*if (!serverReply.empty())
                    {
                        // In this case, the client socket receives only
                        // one string "registered" or "unregistered" as the reply.

                        if (serverReply == "registered")
                        {
                            m_loginStatus = true;
                            m_onlineStatus = Status::Online;

                            //std::cout << "Enter the name of the person you want to chat with : ";
                            //std::string name;
                            //std::getline(std::cin, name);
                            //m_friend = name;
                            m_networkManager.setSocketBlocking(false);
                            //m_ui.insertNewFriendTab(name);
                            m_ui.setChatUsername(m_username);
                            m_ui.changeScreenState(ScreenState::ChatScreen);
                        }

                        else if (serverReply == "unregistered")
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Error!");
                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
                            m_ui.m_messageLabel->setText("Can't recognize the username-password combination!\nIf you're not registered with us, please consider signing up.");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: You are not registered with us! Please register before logging in!" << std::endl;
                            m_networkManager.reset();

                            return false;
                        }

                        else
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Error!");
                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
                            m_ui.m_messageLabel->setText("Unkown reply from server. Please try again later");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: You are not registered with us! Please register before logging in!" << std::endl;
                            m_networkManager.reset();

                            return false;
                        }
                    }*/
                }

                else
                {
                    m_ui.m_messageWindow->setSize(400, 60);
                    m_ui.m_messageWindow->setTitle("Error!");
                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                    m_ui.m_messageLabel->setText("Unable to send packet(s) to the server! Please make sure that the\n     server is up and you're connected to the internet.");
                    m_ui.m_messageLabel->setTextSize(12);
                    m_ui.m_messageLabel->setPosition(20, 10);

                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->show();

                    std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to send login info to server!" << std::endl;
                    m_networkManager.reset();

                    return false;
                }
            }

            else
            {
                m_ui.m_messageWindow->setSize(400, 60);
                m_ui.m_messageWindow->setTitle("Error!");
                if (!m_ui.m_messageWindow->isVisible())
                    m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                m_ui.m_messageLabel->setText("Can't leave either field blank!");
                m_ui.m_messageLabel->setTextSize(12);
                m_ui.m_messageLabel->setPosition(20, 10);

                if (!m_ui.m_messageWindow->isVisible())
                    m_ui.m_messageWindow->show();

                std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Username or password field is blank!" << std::endl;
                m_networkManager.reset();
            }
        }

        else
        {
            m_ui.m_messageWindow->setSize(400, 60);
            m_ui.m_messageWindow->setTitle("Error!");
            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
            m_ui.m_messageLabel->setText("Unable to connect to server! Please make sure that the\n     server is up and you're connected to the internet.");
            m_ui.m_messageLabel->setTextSize(12);
            m_ui.m_messageLabel->setPosition(20, 10);

            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->show();

            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Can't connect to server! Please try again" << std::endl;
            m_networkManager.reset();

            return false;
        }
    }

    bool Client::signup()
    {
        /*if (m_networkManager.connectToServer(SERVER_IP_ADDRESS, OPEN_PORT))
        {
            //if (m_username != "" && m_password != "")
            if (!checkIfWhitespace(m_username) && !checkIfWhitespace(m_password))
            {
                if (m_networkManager.send(m_username, m_password, "new_user"))
                {
                    std::string serverReply;
                    m_networkManager.receive(serverReply);

                    std::cout << serverReply << std::endl;

                    if (!serverReply.empty())
                    {
                        // In this case, the client socket receives only
                        // one string "registered" as the reply. Any other
                        // reply would be counted as error.

                        if (serverReply == "registered")
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Registered!");
                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
                            m_ui.m_messageLabel->setText("Registration successful! Now login to start prattling! ;)");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            //std::cout << "Registration successful! Now login to start prattling!" << std::endl;
                            reset();
                            m_ui.changeScreenState(ScreenState::LoginScreen);

                            return true;
                        }

                        else
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Error!");
                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
                            m_ui.m_messageLabel->setText("Registration was not successful! :(\n     Please try again.");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            //std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Registration was not successful! Please try again." << std::endl;
                            m_networkManager.reset();

                            return false;
                        }
                    }
                }

                else
                {
                    m_ui.m_messageWindow->setSize(400, 60);
                    m_ui.m_messageWindow->setTitle("Error!");
                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
                    m_ui.m_messageLabel->setText("Unable to send login info to server! Please try again.");
                    m_ui.m_messageLabel->setTextSize(12);
                    m_ui.m_messageLabel->setPosition(20, 10);

                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->show();

                    std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to send login info to server!" << std::endl;
                    m_networkManager.reset();

                    return false;
                }
            }

            else
            {
                m_ui.m_messageWindow->setSize(400, 60);
                    m_ui.m_messageWindow->setTitle("Error!");
                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
                    m_ui.m_messageLabel->setText("Can't leave eitherfield blank!");
                    m_ui.m_messageLabel->setTextSize(12);
                    m_ui.m_messageLabel->setPosition(20, 10);

                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->show();

                std::cerr << __FILE__ << ":" << __LINE__ << "ERROR :: Username and password fields can't be blank!" << std::endl;
                m_networkManager.reset();
            }
        }

        else
        {
            m_ui.m_messageWindow->setSize(400, 60);
            m_ui.m_messageWindow->setTitle("Error!");
            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
            m_ui.m_messageLabel->setText("Can't connect to server! Please make sure the server is up\nand running, you're connected to the internet and try again.");
            m_ui.m_messageLabel->setTextSize(12);
            m_ui.m_messageLabel->setPosition(20, 10);

            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->show();

            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Can't connect to server! Please try again." << std::endl;
            m_networkManager.reset();

            return false;
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

    //void Client::searchUsername(const std::string& username)
    void Client::searchUsername()
    {
        /*m_networkManager.setSocketBlocking(true);

        std::string searchQuery = m_ui.getSearchBoxText();

        if (m_networkManager.send(m_username, "server", searchQuery))
        {
            std::string serverReply;
            m_networkManager.receive(serverReply);

            if (!serverReply.empty())
            {
                // In this case, the client socket receives only
                // one string "legal_result" or NOTHING.

                if (serverReply == "legal_result")
                {
                    m_ui.m_searchResults->setSize(m_ui.m_searchResults->getSize().x, m_ui.m_searchResults->getSize().y + 20);
                    m_ui.m_searchResults->addItem(searchQuery);
                }

                else
                {
                    m_ui.m_searchMsg->setText("No matching results found!");
                }
            }
            else
            {
                m_ui.m_messageWindow->setSize(400, 60);
                m_ui.m_messageWindow->setTitle("Error!");
                if (!m_ui.m_messageWindow->isVisible())
                    m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
                m_ui.m_messageLabel->setText("Faulty packet received from Server! Please try again.");
                m_ui.m_messageLabel->setTextSize(12);
                m_ui.m_messageLabel->setPosition(20, 10);

                if (!m_ui.m_messageWindow->isVisible())
                    m_ui.m_messageWindow->show();

                std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;
                m_networkManager.reset();
            }
        }

        else
        {
            m_ui.m_messageWindow->setSize(400, 60);
            m_ui.m_messageWindow->setTitle("Error!");
            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
            m_ui.m_messageLabel->setText("Unable to send packet(s) to the server! Please make sure that the\n     server is up and you're connected to the internet.");
            m_ui.m_messageLabel->setTextSize(12);
            m_ui.m_messageLabel->setPosition(20, 10);

            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->show();

            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to send query to server!" << std::endl;
            m_networkManager.reset();
        }

        m_networkManager.setSocketBlocking(false);*/
    }
}

