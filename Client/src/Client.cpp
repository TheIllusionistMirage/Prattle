#include "../include/Client.hpp"

namespace prattle
{
    Client::Client() : m_networkManager{}
                     , m_ui{}
                     , m_loginStatus{false}
                     , m_onlineStatus{Status::Offline}
                     , m_username{""}
                     , m_password{""}
    {
        //m_windowPtr = m_ui.getRenderWindow();
        //m_guiPtr = m_ui.getGui();
        m_networkManager.reset();
        m_ui.reset();

        m_ui.m_loginButton->connect("pressed", &Client::login, this);
        m_ui.m_submitButton->connect("pressed", &Client::signup, this);
        m_ui.m_logoutButton->connect("pressed", &Client::logout, this);
        m_ui.m_searchButton->connect("pressed", &Client::searchUsername, this);
        m_ui.m_addFriendButton->connect("pressed", &Client::addFriend, this);
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
        while (m_ui.getRenderWindow()->isOpen())
        {
            sf::Event event;

            while (m_ui.getRenderWindow()->pollEvent(event))
            {
                m_ui.getGui()->handleEvent(event);

                if (event.type == sf::Event::Closed)
                    m_ui.getRenderWindow()->close();

                /*if (event.type == sf::Event::Resized)
                {
                    m_ui.getRenderWindow()->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));

                    // Problem in scaling background
                    m_ui.getBackground()->setSize(event.size.width, event.size.height);
                    //m_ui.getBackground()->scale(event.size.width / m_ui.getBackground()->getSize().x,
                                                //event.size.height / m_ui.getBackground()->getSize().y);

                    m_ui.getGui()->setView(m_ui.getRenderWindow()->getView());
                }*/

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
                {
                    if (isLoggedIn())
                    {
                        if (!checkIfWhitespace(m_ui.getInputText()))
                        {
                            std::string message = m_ui.getInputText();
                            //std::string message = m_username + " : " + m_ui.getInputText() + "\n";
                            //std::cout << "xxx " << m_username << " : " << message << " xxx" << std::endl;

                            sf::Packet packet;
                            //LOG(m_ui.getFriendTabPtr()->getSelected());
                            std::string frnd = m_ui.getFriendTabPtr()->getSelected();
                            packet << SEND_MSG << m_username << frnd << message;
                            //LOG(SEND_MSG + m_username + m_ui.getFriendTabPtr()->getSelected() + message);

                            if(m_networkManager.send(packet))
                            {
                                sf::Packet replyPacket;

                                if (m_networkManager.receive(replyPacket))
                                {
                                    std::string protocol, sender, receiver, details;

                                    if (replyPacket >> protocol)
                                    {
                                        if (protocol == SEND_MSG_SUCCESS)
                                            LOG("Msg sent");
                                        else if (protocol == SEND_MSG_FAILURE)
                                        {
                                            replyPacket >> sender >> receiver >> details;
                                            LOG("Msg not sent. Details : " + details);
                                        }
                                    }

                                    else
                                        LOG("Corrupt packet received!");

                                    //m_ui.addTextToChatBox(m_username, message);
                                    //m_ui.addTextToChatBox(m_username, message);
                                    //std::cout << m_username << " : " << message << std::endl;
                                    //m_ui.clearInputTextBox();
                                }

                                m_ui.addTextToChatBox(m_username, message);
                                //m_ui.addTextToChatBox(m_username, message);
                                //std::cout << m_username << " : " << message << std::endl;
                                m_ui.clearInputTextBox();
                                //m_ui.clearInputTextBox();
                            }

                            else
                            {
                                LOG("Unable to send message to server!");
                            }
                        }
                    }
                }

                //m_ui.getGui()->handleEvent(event);
            }

            if (isLoggedIn())
            {
                std::string protocol, sender, source, receiver, message;

                sf::Packet packet;
                while (m_networkManager.receive(packet))
                {
                    //std::cout << protocol << source << sender << receiver << message << std::endl;
                    packet >> protocol;

                    if (protocol ==  SEND_MSG)
                    {
                        packet >> source >> receiver >> sender >> message;
                        m_ui.addTextToChatBox(sender, message);
                    }
                }
            }

            m_ui.updateWidgets();

            m_username = m_ui.getUsernameFieldText();
            m_password = m_ui.getPasswordFieldText();

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
        //LOG("1");
        if (m_networkManager.connectToServer(SERVER_IP_ADDRESS, OPEN_PORT))
        {
            //LOG("2");
            if (!checkIfWhitespace(m_username) && !checkIfWhitespace(m_password))
            {
                //LOG("3");
                sf::Packet packet;
                packet << LOGIN << m_username << SERVER << m_password ;

                if (m_networkManager.send(packet))
                {
                    //LOG("4");
                    sf::Packet replyPacket;
                    m_networkManager.receive(replyPacket);

                    std::string protocol, sender, user, frnd;
                    unsigned int friendCount;
                    std::vector<std::string> friends;

                    //std::cout << "abcdefgh ----" << std::endl;

                    if (replyPacket >> protocol)
                    {
                        //LOG("5");
                        //std::cout << protocol << std::endl;
                        LOG(protocol);

                        if (protocol == LOGIN_SUCCESS)
                        {
                            //std::cout << protocol << " " << sender << " " << user << " " << friendCount;

                            if(replyPacket >> sender >> user >> friendCount)
                            {
                                //LOG(protocol + " " + sender + " " + user + " " + std::to_string(friendCount));
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

                        else
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Error!");
                            if (!m_ui.m_messageWindow->isVisible())
                                //m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_guiPtr) / 2 - 200, tgui::bindHeight(*m_guiPtr) / 2 - m_ui.m_messageLabel->getSize().y);
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                            m_ui.m_messageLabel->setText("Unrecognized protocol received!");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unknown protocol!" << std::endl;
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
                        m_ui.m_messageLabel->setText("Damaged packet received from server. Please try again later");
                        m_ui.m_messageLabel->setTextSize(12);
                        m_ui.m_messageLabel->setPosition(20, 10);

                        if (!m_ui.m_messageWindow->isVisible())
                            m_ui.m_messageWindow->show();

                        std::cerr << __FILE__ << ':' << __LINE__ << " Damaged packet received from server. Please try again later" << std::endl;
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

                return false;
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
        if (m_networkManager.connectToServer(SERVER_IP_ADDRESS, OPEN_PORT))
        {
            if (!checkIfWhitespace(m_username) && !checkIfWhitespace(m_password))
            {
                sf::Packet packet;
                packet << SIGNUP << m_username << SERVER << m_password;

                if (m_networkManager.send(packet))
                {
                    sf::Packet replyPacket;
                    m_networkManager.receive(replyPacket);

                    std::string protocol, sender, receiver, details;

                    if (replyPacket >> protocol)
                    {
                        if (protocol == SIGNUP_SUCCESS)
                        {
                            if (replyPacket >> sender >> receiver)
                            {
                                m_ui.m_messageWindow->setSize(400, 60);
                                m_ui.m_messageWindow->setTitle("Registered!");
                                if (!m_ui.m_messageWindow->isVisible())
                                    m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
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
                                    m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                                m_ui.m_messageLabel->setText("Corrupted packet received!");
                                m_ui.m_messageLabel->setTextSize(12);
                                m_ui.m_messageLabel->setPosition(20, 10);

                                if (!m_ui.m_messageWindow->isVisible())
                                    m_ui.m_messageWindow->show();

                                std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Corrupted packet received." << std::endl;
                                m_networkManager.reset();

                                return false;
                            }
                        }

                        else if (protocol == SIGNUP_FAILURE)
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Error!");
                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                            m_ui.m_messageLabel->setText("Error in signup process! Please try again!");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to signup." << std::endl;
                            m_networkManager.reset();

                            return false;
                        }

                        else
                            return false;
                    }

                    else
                    {
                        m_ui.m_messageWindow->setSize(400, 60);
                        m_ui.m_messageWindow->setTitle("Error!");
                        if (!m_ui.m_messageWindow->isVisible())
                            m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                        m_ui.m_messageLabel->setText("Corrupted packet received!");
                        m_ui.m_messageLabel->setTextSize(12);
                        m_ui.m_messageLabel->setPosition(20, 10);

                        if (!m_ui.m_messageWindow->isVisible())
                            m_ui.m_messageWindow->show();

                        std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Corrupted packet received." << std::endl;
                        m_networkManager.reset();

                        return false;
                    }

                    /*if (!serverReply.empty())
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
                    }*/
                }

                else
                {
                    m_ui.m_messageWindow->setSize(400, 60);
                    m_ui.m_messageWindow->setTitle("Error!");
                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
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
                        m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                    m_ui.m_messageLabel->setText("Can't leave either field blank!");
                    m_ui.m_messageLabel->setTextSize(12);
                    m_ui.m_messageLabel->setPosition(20, 10);

                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->show();

                std::cerr << __FILE__ << ":" << __LINE__ << "ERROR :: Username and password fields can't be blank!" << std::endl;
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
            m_ui.m_messageLabel->setText("Can't connect to server! Please make sure the server is up\nand running, you're connected to the internet and try again.");
            m_ui.m_messageLabel->setTextSize(12);
            m_ui.m_messageLabel->setPosition(20, 10);

            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->show();

            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Can't connect to server! Please try again." << std::endl;
            m_networkManager.reset();

            return false;
        }
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

    /*std::string const& Client::getFriendsName() const
    {
        return m_friend;
    }*/

    bool Client::logout()
    {
        reset();
    }

    //void Client::searchUsername(const std::string& username)
    bool Client::searchUsername(const std::string& username)
    {
        m_networkManager.setSocketBlocking(true);
        //std::string protocol, sender = m_username, receiver = SERVER;
        std::string searchQuery = m_ui.getSearchBoxText();

        sf::Packet packet;
        packet << SEARCH_USER << m_username << SERVER << searchQuery;

        if (m_networkManager.send(packet))
        {
            sf::Packet replyPacket;
            std::string protocol, sender, receiver, result;

            m_networkManager.receive(replyPacket);

            //LOG("ASA");
            if (replyPacket >> protocol)
            {
                //LOG("ASA -");
                if (protocol == SEARCH_USER_RESULTS)
                {
                    //LOG("ASA --");
                    if (replyPacket >> sender >> receiver >> result)
                    {
                        //LOG("ASA ---" + result);
                        //LOG(result);
                        if (result == "")
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Search Info");
                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                            m_ui.m_messageLabel->setText("\'" + searchQuery + "\' is not registered!");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            m_ui.m_searchResults->setSize(260, 0);
                            m_ui.m_searchResults->removeAllItems();

                            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;
                            //m_networkManager.reset();

                            m_networkManager.setSocketBlocking(false);

                            return true;
                        }

                        else
                        {
                            //std::cout << "ASA ---+" << std::endl;
                            m_ui.m_searchResults->setSize(260, 0);
                            m_ui.m_searchResults->removeAllItems();
                            m_ui.m_searchResults->setSize(m_ui.m_searchResults->getSize().x, m_ui.m_searchResults->getSize().y + 20);
                            m_ui.m_searchResults->addItem(searchQuery);

                            m_networkManager.setSocketBlocking(false);
                            //m_networkManager.reset();

                            return true;
                        }
                    }

                    else
                    {
                        m_ui.m_messageWindow->setSize(400, 60);
                        m_ui.m_messageWindow->setTitle("Error!");
                        if (!m_ui.m_messageWindow->isVisible())
                            m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                        m_ui.m_messageLabel->setText("Corrupt packet received from server!");
                        m_ui.m_messageLabel->setTextSize(12);
                        m_ui.m_messageLabel->setPosition(20, 10);

                        if (!m_ui.m_messageWindow->isVisible())
                            m_ui.m_messageWindow->show();

                        std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;
                        //m_networkManager.reset();

                        m_networkManager.setSocketBlocking(false);

                        return false;
                    }
                }

                else
                {
                    m_ui.m_messageWindow->setSize(400, 60);
                    m_ui.m_messageWindow->setTitle("Error!");
                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                    m_ui.m_messageLabel->setText("Corrupt packet received from server!");
                    m_ui.m_messageLabel->setTextSize(12);
                    m_ui.m_messageLabel->setPosition(20, 10);

                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->show();

                    std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;
                    //m_networkManager.reset();

                    m_networkManager.setSocketBlocking(false);

                    return false;
                }
            }

            else
            {
                m_ui.m_messageWindow->setSize(400, 60);
                m_ui.m_messageWindow->setTitle("Error!");
                if (!m_ui.m_messageWindow->isVisible())
                    m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                m_ui.m_messageLabel->setText("Corrupt packet received from server!");
                m_ui.m_messageLabel->setTextSize(12);
                m_ui.m_messageLabel->setPosition(20, 10);

                if (!m_ui.m_messageWindow->isVisible())
                    m_ui.m_messageWindow->show();

                std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;
                //m_networkManager.reset();

                m_networkManager.setSocketBlocking(false);

                return false;
            }
        }

        else
        {
            m_ui.m_messageWindow->setSize(400, 60);
            m_ui.m_messageWindow->setTitle("Error!");
            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
            m_ui.m_messageLabel->setText("Unable to send search query to server! Please try again.");
            m_ui.m_messageLabel->setTextSize(12);
            m_ui.m_messageLabel->setPosition(20, 10);

            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->show();

            std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;
            //m_networkManager.reset();

            m_networkManager.setSocketBlocking(false);

            return false;
        }
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

    //void Client::addFriend(const std::string& username)
    void Client::addFriend()
    {
        m_networkManager.setSocketBlocking(true);

        std::string username = m_ui.m_searchResults->getSelectedItem();
        sf::Packet packet;
        packet << ADD_FRIEND << m_username << SERVER << username;

        if (m_networkManager.send(packet))
        {
            std::string protocol, sender, receiver, details;
            sf::Packet replyPacket;

            m_networkManager.receive(replyPacket);

            if (replyPacket >> protocol)
            {
                if (protocol == ADD_FRIEND_SUCCESS)
                {
                    m_ui.insertNewFriend(username);

                    m_ui.m_messageWindow->setSize(400, 60);
                    m_ui.m_messageWindow->setTitle("Info");
                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                    m_ui.m_messageLabel->setText("\'" + username + "\' successfully added as your friend! To chat with him, double click his name\nfrom your buddy list");
                    m_ui.m_messageLabel->setTextSize(12);
                    m_ui.m_messageLabel->setPosition(20, 10);

                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->show();

                    //std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;

                    m_networkManager.setSocketBlocking(false);
                }

                else if (protocol == ADD_FRIEND_FAILURE)
                {
                    if (replyPacket >> sender >> receiver >> details)
                    {
                        m_ui.m_messageWindow->setSize(400, 60);
                        m_ui.m_messageWindow->setTitle("Error!");
                        if (!m_ui.m_messageWindow->isVisible())
                            m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                        m_ui.m_messageLabel->setText("\'" + username + "\' was not added as your friend! " + details);
                        m_ui.m_messageLabel->setTextSize(12);
                        m_ui.m_messageLabel->setPosition(20, 10);

                        if (!m_ui.m_messageWindow->isVisible())
                            m_ui.m_messageWindow->show();

                        //std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;

                        m_networkManager.setSocketBlocking(false);
                    }
                }

                else
                {
                    m_ui.m_messageWindow->setSize(400, 60);
                    m_ui.m_messageWindow->setTitle("Error!");
                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                    m_ui.m_messageLabel->setText("Unrecognized protocol received from the server.");
                    m_ui.m_messageLabel->setTextSize(12);
                    m_ui.m_messageLabel->setPosition(20, 10);

                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->show();

                    //std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;

                    m_networkManager.setSocketBlocking(false);
                }
            }

            else
            {
                m_ui.m_messageWindow->setSize(400, 60);
                m_ui.m_messageWindow->setTitle("Error!");
                if (!m_ui.m_messageWindow->isVisible())
                    m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
                m_ui.m_messageLabel->setText("Corrupt packet received from the server.");
                m_ui.m_messageLabel->setTextSize(12);
                m_ui.m_messageLabel->setPosition(20, 10);

                if (!m_ui.m_messageWindow->isVisible())
                    m_ui.m_messageWindow->show();

                //std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;

                m_networkManager.setSocketBlocking(false);
            }
        }

        else
        {
            m_ui.m_messageWindow->setSize(400, 60);
            m_ui.m_messageWindow->setTitle("Error!");
            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);
            m_ui.m_messageLabel->setText("Unable to send friend request protocol to server! Please try again.");
            m_ui.m_messageLabel->setTextSize(12);
            m_ui.m_messageLabel->setPosition(20, 10);

            if (!m_ui.m_messageWindow->isVisible())
                m_ui.m_messageWindow->show();

            //std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Unable to receive search results from the server" << std::endl;

            m_networkManager.setSocketBlocking(false);
        }
    }
}
