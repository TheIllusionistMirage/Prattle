/**

    Prattle/Client/Client.cpp
    =====================

    Contains implementations of class Client declared in Prattle/Client/Client.hpp

*/

#include "../include/Client.hpp"

namespace prattle
{
    //Client::Client(std::string s_ip, int s_port) : m_loginStatus{false}
    Client::Client() : m_server_ip{""}
                     , m_server_port{-1}
                     , m_loginStatus{false}
                     , m_username{""}
                     , m_password{""}
                     , m_onlineStatus{Status::Offline}
                     , m_networkManager{}
                     , m_ui{}
                     , m_configFile{CLIENT_CONFIG_FILE, std::ios::in}
    {
        if (m_configFile.is_open() && m_configFile.good())
        {
            parseConfigFile();
        }

        if (m_server_ip == "" || m_server_port == -1)
        {
            LOG("FATAL ERROR :: Server IP address or server's open port unspecified! Please check \'" + CLIENT_CONFIG_FILE + "\' for any erroneous values.");
            throw std::runtime_error("FATAL ERROR :: Server IP address or server's open port unspecified! Please check \'" + CLIENT_CONFIG_FILE + "\' for any erroneous values.");
        }

        m_networkManager.reset();
        m_ui.reset();

        m_ui.m_loginButton->connect("pressed", &Client::login, this);
        m_ui.m_submitButton->connect("pressed", &Client::signup, this);
        m_ui.m_logoutButton->connect("pressed", &Client::logout, this);
        m_ui.m_searchButton->connect("pressed", &Client::searchUsername, this);
        m_ui.m_addFriendButton->connect("pressed", &Client::addFriend, this);
    }

    void Client::parseConfigFile()
    {
        if (m_configFile.is_open() && m_configFile.good())
        {
            m_configFile.seekg(std::ios::beg);
            std::string line;
            std::getline(m_configFile, line);

            for (unsigned int i = 1; !m_configFile.eof(); std::getline(m_configFile, line), i++)
            {
                std::string field;
                std::string value;

                if(line[0] == '#' || line.size() < 2)
                    continue;

                auto first_colon = line.find(':', 0);
                auto second_colon = line.find(':', first_colon + 1);

                if(first_colon == std::string::npos
                   || second_colon == std::string::npos)
                {
                    LOG("WARNING :: Invalid configuration value at " + CLIENT_CONFIG_FILE + " : " + std::to_string(i));
                    continue;
                }

                field = line.substr(0, first_colon);
                value = line.substr(first_colon + 1, second_colon - first_colon - 1);

                if (field == "SERVER_IP")
                    m_server_ip = value;

                else if (field == "OPEN_PORT")
                    m_server_port = std::stoi(value);

            }
        }
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
                {
                    m_ui.getRenderWindow()->close();

                    //if ()
                }

                if (event.type == sf::Event::Resized)
                {
                    m_ui.getRenderWindow()->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                    m_ui.getGui()->setView(m_ui.getRenderWindow()->getView());
                }

                /*if (event.type == sf::Event::LostFocus || event.type == sf::Event::GainedFocus)
                {

                }*/

                bool shift = false;

                if ((event.type == sf::Event::KeyPressed &&
                      event.key.code == sf::Keyboard::Return) &&
                       event.key.shift)
                    shift = true;

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return && !shift)
                {
                    if (isLoggedIn())
                    {
                        if (!checkIfWhitespace(m_ui.getInputText()))
                        {
                            std::string message = m_ui.getInputText();
                            sf::Packet packet;
                            //std::string frnd = m_ui.getFriendTabPtr()->getSelected();
                            std::string frnd = m_ui.getSelectedFriendTab();
                            //std::cout << frnd.substr(0, 1) + "|" << std::endl;
                            //std::cout << frnd.substr(2, frnd.size()) << std::endl;
                            if (frnd.substr(0, 2) == "* ")
                            {
                                //std::cout << "A" << std::endl;
                                frnd = frnd.substr(2, frnd.size());
                            }
                            packet << SEND_MSG << m_username << frnd << message;

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
                                }

                                m_ui.addTextToChatBox(m_username + " : " + message);
                                m_ui.clearInputTextBox();

                                //system("clear");
                                //std::cout << m_ui.getSelectedFriendTab() << std::endl;

                                //std::cout << frnd << std::endl;
                                m_ui.m_chatHistory.find(frnd)->second = m_ui.m_chatHistory.find(frnd)->second + m_username + " : " + message;// + "\n";
                            }

                            else
                            {
                                LOG("Unable to send message to server!");
                            }
                        }
                    }

                    else
                    {
                        if (!checkIfWhitespace(m_ui.getUsernameFieldText())
                             && !checkIfWhitespace(m_ui.getPasswordFieldText()))
                            login();
                    }
                }
            }

            if (isLoggedIn())
            {
                std::string protocol, sender, source, receiver, message;
                sf::Packet packet;

                while (m_networkManager.receive(packet))
                {
                    packet >> protocol;

                    if (protocol ==  SEND_MSG)
                    {
                        //packet >> source >> receiver >> sender >> message;
                        //m_ui.addTextToChatBox(sender, message);

                        if (packet >> source >> receiver >> sender >> message)
                        {
                            //std::cout << sender << std::endl;
                            m_ui.m_chatHistory.find(sender)->second = m_ui.m_chatHistory.find(sender)->second + sender + " : " + message;

                            std::string str = m_ui.getSelectedFriendTab();
                            //std::cout << str << std::endl;
                            if (str.substr(0, 2) == "* ")
                            {
                                if (str.substr(2, str.size() - 1) == sender)
                                {
                                    m_ui.addTextToChatBox(sender + " : " + message);
                                }
                                else
                                {
                                    m_ui.insertNotification(sender);
                                }
                            }
                            else if (str == sender)
                                m_ui.addTextToChatBox(sender + " : " + message);
                            else
                                m_ui.insertNotification(sender);

                            //auto chatter = m_ui.getSelectedFriendTab();
                            //m_chatHistory.find(chatter)->second = m_chatHistory.find(chatter)->second + sender + " : " + message;// + "\n";

                            //m_chatHistory.find(chatter)->second = m_chatHistory.find(chatter)->second + message;
                            //std::cout << m_chatHistory.find(chatter)->second << std::endl;
                            //m_ui.m_chatHistory.find(sender)->second = m_ui.m_chatHistory.find(sender)->second + sender + " : " + message;// + "\n";
                            /*if (m_ui.getSelectedFriendTab() != sender)
                            {
                                m_ui.insertNotification(sender);
                            }*/
                            //m_ui.clearChatBox();
                            //m_ui.addTextToChatBox(chatter, m_chatHistory.find(chatter)->second);
                        }
                    }

                    else if (protocol == ADD_FRIEND_SUCCESS)
                    {
                        if (packet >> source >> receiver >> sender)
                            m_ui.insertNewFriend(sender);
                    }
                }
            }

            //m_ui.getFriendTabPtr();
            //LOG(m_ui.getSelectedFriendTab());

            /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
            {
                if (!checkIfWhitespace(m_ui.getUsernameFieldText())
                     && !checkIfWhitespace(m_ui.getPasswordFieldText()))
                    login();
            }*/

            /*if (m_ui.getSelectedFriendTab() != "")
            {
                m_ui.clearChatBox();
                //std::string name = m_ui.getSelectedFriendTab();
                //if (name.substr(0, 2) == "(*)")
                    //m_ui.addTextToChatBox(m_chatHistory.find(name.substr(0, 2))->second);

                m_ui.addTextToChatBox(m_chatHistory.find(m_ui.getSelectedFriendTab())->second);
            }*/
            /*m_ui.clearChatBox();
            std::string s = m_ui.getSelectedFriendTab();
            std::string s2 = m_chatHistory.find(s)->second;
            m_ui.addTextToChatBox(s, s2);*/
            //system("clear");
            //std::string s = m_ui.getSelectedFriendTab();
            //std::cout << m_chatHistory.find(s)->second << std::endl;

            //std::cout << m_chatHistory.find(m_ui.getSelectedFriendTab())->second << std::endl;

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

    bool Client::login()
    {
        if (m_networkManager.connectToServer(m_server_ip, m_server_port))
        {
            if (!checkIfWhitespace(m_username) && !checkIfWhitespace(m_password))
            {
                sf::Packet packet;
                packet << LOGIN << m_username << SERVER << m_password;

                if (m_networkManager.send(packet))
                {
                    sf::Packet replyPacket;
                    m_networkManager.receive(replyPacket);

                    std::string protocol, sender, user, frnd;
                    unsigned int friendCount;
                    std::vector<std::string> friends;

                    if (replyPacket >> protocol)
                    {
                        if (protocol == LOGIN_SUCCESS)
                        {
                            if(replyPacket >> sender >> user >> friendCount)
                            {
                                for (unsigned int i = 1; i <= friendCount; ++i)
                                {
                                    replyPacket >> frnd;
                                    friends.push_back(frnd);
                                }

                                m_friends = friends;

                                for (auto itr : m_friends)
                                {
                                    m_ui.m_chatHistory[itr] = "";
                                }

                                /*for (auto itr : m_friends)
                                {
                                    std::cout << m_chatHistory.find(itr)->second << " ";
                                }
                                std::cout << std::endl;*/

                                m_loginStatus = true;
                                m_onlineStatus = Status::Online;

                                m_networkManager.setSocketBlocking(false);
                                m_ui.setChatUsername(m_username);
                                m_ui.initFriendList(m_friends);
                                m_ui.changeScreenState(ScreenState::ChatScreen);

                                return true;
                            }

                            else
                            {
                                m_ui.m_messageWindow->setSize(400, 60);
                                m_ui.m_messageWindow->setTitle("Error!");

                                if (!m_ui.m_messageWindow->isVisible())
                                    m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);

                                m_ui.m_messageLabel->setText("Corrupt packet received!");
                                m_ui.m_messageLabel->setTextSize(12);
                                m_ui.m_messageLabel->setPosition(20, 10);

                                if (!m_ui.m_messageWindow->isVisible())
                                    m_ui.m_messageWindow->show();

                                LOG("ERROR :: Corrupt packet received from server");
                                m_networkManager.reset();

                                return false;
                            }
                        }

                        else if (protocol == LOGIN_FAILURE)
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Error!");

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);

                            m_ui.m_messageLabel->setText("Can't recognize the username-password combination!\nIf you're not registered with us, please consider signing up.");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            LOG("ERROR :: You are not registered with us! Please register before logging in!");
                            m_networkManager.reset();

                            return false;
                        }

                        else
                        {
                            m_ui.m_messageWindow->setSize(400, 60);
                            m_ui.m_messageWindow->setTitle("Error!");

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->setPosition(tgui::bindWidth(*m_ui.getGui()) / 2 - 200, tgui::bindHeight(*m_ui.getGui()) / 2 - m_ui.m_messageLabel->getSize().y);

                            m_ui.m_messageLabel->setText("Unrecognized protocol received!");
                            m_ui.m_messageLabel->setTextSize(12);
                            m_ui.m_messageLabel->setPosition(20, 10);

                            if (!m_ui.m_messageWindow->isVisible())
                                m_ui.m_messageWindow->show();

                            LOG("ERROR :: Unknown protocol!");
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

                        LOG("Damaged packet received from server. Please try again later");
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

                    LOG("ERROR :: Unable to send login info to server!");
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

                LOG("ERROR :: Username or password field is blank!");
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

            LOG(" ERROR :: Can't connect to server! Please try again");
            m_networkManager.reset();

            return false;
        }
    }

    bool Client::signup()
    {
        if (m_networkManager.connectToServer(m_server_ip, m_server_port))
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

                                LOG("Registration successful! Now login to start prattling!");
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

                                LOG("ERROR :: Corrupted packet received.");
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

                            LOG("ERROR :: Unable to signup.");
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

                        LOG("ERROR :: Corrupted packet received.");
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

                    m_ui.m_messageLabel->setText("Unable to send login info to server! Please try again.");
                    m_ui.m_messageLabel->setTextSize(12);
                    m_ui.m_messageLabel->setPosition(20, 10);

                    if (!m_ui.m_messageWindow->isVisible())
                        m_ui.m_messageWindow->show();

                    LOG("ERROR :: Unable to send login info to server!");
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

                LOG("ERROR :: Username and password fields can't be blank!");
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

            LOG(" ERROR :: Can't connect to server! Please try again.");
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

    void Client::logout()
    {
        reset();
    }

    bool Client::searchUsername(const std::string& username)
    {
        m_networkManager.setSocketBlocking(true);
        std::string searchQuery = m_ui.getSearchBoxText();

        sf::Packet packet;
        packet << SEARCH_USER << m_username << SERVER << searchQuery;

        if (m_networkManager.send(packet))
        {
            sf::Packet replyPacket;
            std::string protocol, sender, receiver, result;

            m_networkManager.receive(replyPacket);

            if (replyPacket >> protocol)
            {
                if (protocol == SEARCH_USER_RESULTS)
                {
                    if (replyPacket >> sender >> receiver >> result)
                    {
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

                            LOG("ERROR :: Unable to receive search results from the server");
                            m_networkManager.setSocketBlocking(false);

                            return true;
                        }

                        else
                        {
                            m_ui.m_searchResults->setSize(260, 0);
                            m_ui.m_searchResults->removeAllItems();
                            m_ui.m_searchResults->setSize(m_ui.m_searchResults->getSize().x, m_ui.m_searchResults->getSize().y + 20);
                            m_ui.m_searchResults->addItem(searchQuery);

                            m_networkManager.setSocketBlocking(false);

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

                        LOG("ERROR :: Unable to receive search results from the server");
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

                    LOG("ERROR :: Unable to receive search results from the server");
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

                LOG("ERROR :: Unable to receive search results from the server");
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

            LOG("ERROR :: Unable to receive search results from the server");
            m_networkManager.setSocketBlocking(false);

            return false;
        }
    }

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

                    LOG("ERROR :: Unable to receive search results from the server");
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

                        LOG("ERROR :: Unable to receive search results from the server");
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

                    LOG("ERROR :: Unable to receive search results from the server");
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

                LOG("ERROR :: Unable to receive search results from the server");
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

            LOG("ERROR :: Unable to receive search results from the server");
            m_networkManager.setSocketBlocking(false);
        }
    }
}
