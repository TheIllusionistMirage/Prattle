#include "../include/ClientGUI.hpp"

namespace prattle
{
    ClientGUI::ClientGUI()
    {
        resetSocket();
        resetGUI();

        m_loginButton->connect("pressed", &Client::login, this);
        m_submitButton->connect("pressed", &Client::signup, this);
        m_logoutButton->connect("pressed", &Client::logout, this);
        m_searchButton->connect("pressed", &Client::searchUsername, this);
        m_addFriendButton->connect("pressed", &Client::addFriend, this);
    }

    void ClientGUI::run(float dt)
    {
        while (getRenderWindow()->isOpen())
        {
            sf::Event event;

            while (getRenderWindow()->pollEvent(event))
            {
                getGui()->handleEvent(event);

                bool shift = false;

                switch(event.type)
                {
                    case sf::Event::Closed:
                        {
                            getRenderWindow()->close();
                        }
                        break;

                    case sf::Event::KeyPressed:
                        {
                            if (event.key.code == sf::Keyboard::F4 && event.key.alt)
                                getRenderWindow()->close();

                            else if (event.key.code == sf::Keyboard::Return)
                            {
                                if (event.key.shift)
                                    shift = true;

                                if (!shift)
                                {
                                    if (isLoggedIn())
                                    {
                                        if (!checkIfWhitespace(getInputText()))
                                        {
                                            std::string message = getInputText();
                                            sf::Packet packet;
                                            std::string selectedTab = getFriendTabPtr()->getSelected();

                                            if (selectedTab .substr(0, 2) == "* ")
                                            {
                                                selectedTab  = selectedTab .substr(2, selectedTab .size());
                                            }

                                            packet << SEND_MSG << m_username << selectedTab  << message;

                                            if(send(packet))
                                            {
                                                sf::Packet replyPacket;

                                                if (receive(replyPacket))
                                                {
                                                    std::string protocol, sender, receiver, details;

                                                    if (replyPacket >> protocol)
                                                    {
                                                        if (protocol == SEND_MSG_SUCCESS)
                                                            LOG("Packet was successfully sent from \'" + m_username + "\' to \'" + selectedTab + "\'");

                                                        else if (protocol == SEND_MSG_FAILURE)
                                                        {
                                                            replyPacket >> sender >> receiver >> details;
                                                            LOG("ERROR :: Unable to send packet from \'" + m_username + "\' to \'" + selectedTab + "\'. Details : " + details);
                                                        }
                                                    }

                                                    else
                                                        LOG("ERROR :: Corrupt packet received from the server.");
                                                }

                                                addTextToChatBox(m_username + " : " + message);
                                                clearInputTextBox();

                                                if (selectedTab != "")
                                                    m_chatHistory.find(selectedTab)->second = m_chatHistory.find(selectedTab)->second + m_username + " : " + message;
                                            }

                                            else
                                            {
                                                LOG("ERROR :: Unable to send packet from \'" + m_username + "\' to the server");
                                            }
                                        }
                                    }

                                    else
                                    {
                                        if (!checkIfWhitespace(getUsernameFieldText())
                                             && !checkIfWhitespace(getPasswordFieldText()))
                                            login();
                                    }
                                }
                            }
                        }
                        break;

                    case sf::Event::Resized:
                        {
                            getRenderWindow()->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                            getGui()->setView(getRenderWindow()->getView());
                        } break;
                }
            }

            if (isLoggedIn())
            {
                std::string protocol, sender, source, receiver, message;
                sf::Packet packet;

                while (receive(packet))
                {
                    packet >> protocol;

                    if (protocol == SEND_MSG)
                    {
                        if (packet >> source >> receiver >> sender >> message)
                        {
                            m_chatHistory.find(sender)->second = m_chatHistory.find(sender)->second + sender + " : " + message;

                            std::string selectedTab = getFriendTabPtr()->getSelected();

                            if (selectedTab.substr(0, 2) == "* ")
                            {
                                if (selectedTab.substr(2, selectedTab.size() - 1) == sender)
                                {
                                   addTextToChatBox(sender + " : " + message);
                                }

                                else
                                {
                                    insertNotification(sender);
                                }
                            }

                            else if (selectedTab == sender)
                            {
                                addTextToChatBox(sender + " : " + message);
                                reloadChat();
                            }

                            else
                                insertNotification(sender);
                        }
                    }

                    else if (protocol == ADD_FRIEND_SUCCESS)
                    {
                        if (packet >> source >> receiver >> sender)
                        {
                            addNewFriend(sender);
                            m_friends.push_back(sender);
                            m_chatHistory[sender] = "";
                        }
                    }
                }
            }

            update();

            m_username = getUsernameFieldText();
            m_password = getPasswordFieldText();

            getRenderWindow()->clear(sf::Color::Black);
            getGui()->draw();
            getRenderWindow()->display();
        }
    }

    void ClientGUI::reset()
    {
        m_username = "";
        m_password = "";
        resetSocket();
        resetGUI();
    }

    bool ClientGUI::login()
    {
        if (connect())
        {
            if (!checkIfWhitespace(m_username) && !checkIfWhitespace(m_password))
            {
                sf::Packet packet;
                packet << LOGIN << m_username << SERVER << m_password;

                if (send(packet))
                {
                    sf::Packet replyPacket;
                    receive(replyPacket);

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
                                    m_chatHistory[itr] = "";
                                }

                                setStatus(Status::Online);

                                blockSocket(false);
                                setChatUsername(m_username);
                                initFriendList(m_friends);
                                setScreenState(ScreenState::ChatScreen);

                                return true;
                            }

                            else
                            {
                                alert("Corrupt packet received!");
                                LOG("ERROR :: Corrupt packet received from server");
                                resetSocket();

                                return false;
                            }
                        }

                        else if (protocol == LOGIN_FAILURE)
                        {
                            alert("Can't recognize the username-password combination!\nIf you're not registered with us, please consider signing up.");
                            LOG("ERROR :: You are not registered with us! Please register before logging in!");
                            resetSocket();

                            return false;
                        }

                        else
                        {
                            alert("Unrecognized protocol received!");
                            LOG("ERROR :: Unknown protocol!");
                            resetSocket();

                            return false;
                        }
                    }

                    else
                    {
                        alert("Damaged packet received from server. Please try again later");
                        LOG("ERROR :: Corrupt packet received from server");
                        resetSocket();

                        return false;
                    }
                }

                else
                {
                    alert("Unable to send packet(s) to the server! Please make sure that the\n     server is up and you're connected to the internet.");
                    LOG("ERROR :: Unable to send login info to server!");
                    resetSocket();

                    return false;
                }
            }

            else
            {
                alert("Can't Leave either username or password field blank!");
                LOG("ERROR :: Username or password field is blank");
                resetSocket();

                return false;
            }
        }

        else
        {
            alert("Unable to connect to server! Please make sure that the\n     server is up and you're connected to the internet.");
            LOG(" ERROR :: Can't connect to server! Please try again");
            resetSocket();

            return false;
        }
    }

    bool ClientGUI::signup()
    {
        if (connect())
        {
            if (!checkIfWhitespace(m_username) && !checkIfWhitespace(m_password))
            {
                sf::Packet packet;
                packet << SIGNUP << m_username << SERVER << m_password;

                if (send(packet))
                {
                    sf::Packet replyPacket;
                    receive(replyPacket);

                    std::string protocol, sender, receiver, details;

                    if (replyPacket >> protocol)
                    {
                        if (protocol == SIGNUP_SUCCESS)
                        {
                            if (replyPacket >> sender >> receiver)
                            {
                                alert("Registration successful! Now login to start prattling!");
                                LOG("\'" + m_username + "\' registered successfully");
                                resetSocket();
                                setScreenState(ScreenState::LoginScreen);

                                return true;
                            }

                            else
                            {
                                alert("Corrupt packet received!");
                                LOG("ERROR :: Corrupt packet received from server");
                                resetSocket();

                                return false;
                            }
                        }

                        else if (protocol == SIGNUP_FAILURE)
                        {
                            alert("Error in signup process! Please try again!");
                            LOG("ERROR :: Unable to signup \'" + m_username + "\', please try again");
                            resetSocket();

                            return false;
                        }

                        else
                            return false;
                    }

                    else
                    {
                        alert("Corrupt packet received from server!");
                        LOG("ERROR :: Corrupt packet received.");
                        resetSocket();

                        return false;
                    }
                }

                else
                {
                    alert("Unable to send login info to server! Please try again.");
                    LOG("ERROR :: Unable to send login info to server!");
                    resetSocket();

                    return false;
                }
            }

            else
            {
                alert("Can't leave either field blank!");
                LOG("ERROR :: Username and password fields can't be blank!");
                resetSocket();

                return false;
            }
        }

        else
        {
            alert("Can't connect to server! Please make sure the server is up\nand running, you're connected to the internet and try again.");
            LOG(" ERROR :: Can't connect to server! Please try again.");
            resetSocket();

            return false;
        }
    }

    void ClientGUI::logout()
    {
        reset();
    }

    bool ClientGUI::searchUsername()
    {
        blockSocket(true);
        std::string searchQuery = getSearchBoxText();

        sf::Packet packet;
        packet << SEARCH_USER << m_username << SERVER << searchQuery;

        if (send(packet))
        {
            sf::Packet replyPacket;
            std::string protocol, sender, receiver, result;

            receive(replyPacket);

            if (replyPacket >> protocol)
            {
                if (protocol == SEARCH_USER_RESULTS)
                {
                    if (replyPacket >> sender >> receiver >> result)
                    {
                        if (result == "")
                        {
                            alert("\'" + searchQuery + "\' is not registered!");
                            LOG("ERROR :: Unable to receive search results from the server");
                            blockSocket(false);

                            return true;
                        }

                        else
                        {
                            m_searchResults->setSize(260, 0);
                            m_searchResults->removeAllItems();
                            m_searchResults->setSize(m_searchResults->getSize().x, m_searchResults->getSize().y + 20);
                            m_searchResults->addItem(searchQuery);

                            blockSocket(false);

                            return true;
                        }
                    }

                    else
                    {
                        alert("Corrupt packet received from server!");
                        LOG("ERROR :: Unable to receive search results from the server");
                        blockSocket(false);

                        return false;
                    }
                }

                else
                {
                    alert("Corrupt packet received from server!");
                    LOG("ERROR :: Unable to receive search results from the server");
                    blockSocket(false);

                    return false;
                }
            }

            else
            {
                alert("Corrupt packet received from server!");
                LOG("ERROR :: Unable to receive search results from the server");
                blockSocket(false);

                return false;
            }
        }

        else
        {
            alert("Unable to send search query to server! Please try again.");
            LOG("ERROR :: Unable to receive search results from the server");
            blockSocket(false);

            return false;
        }
    }

    bool ClientGUI::addFriend()
    {
        blockSocket(true);

        std::string username = m_searchResults->getSelectedItem();
        sf::Packet packet;
        packet << ADD_FRIEND << m_username << SERVER << username;

        if (send(packet))
        {
            std::string protocol, sender, receiver, details;
            sf::Packet replyPacket;

            receive(replyPacket);

            if (replyPacket >> protocol)
            {
                if (protocol == ADD_FRIEND_SUCCESS)
                {
                    m_friends.push_back(username);
                    m_chatHistory[username] = "";

                    addNewFriend(username);

                    m_messageWindow->setSize(400, 60);
                    m_messageWindow->setTitle("Info");

                    alert("\'" + username + "\' successfully added as your friend! To chat with him, double click his name\nfrom your buddy list");
                    LOG("\'" + username + "\' successfully added as \'" + m_username + "\''s friend");
                    blockSocket(false);

                    return true;
                }

                else if (protocol == ADD_FRIEND_FAILURE)
                {
                    if (replyPacket >> sender >> receiver >> details)
                    {
                        alert("\'" + username + "\' was not added as your friend! " + details);
                        LOG("ERROR :: Unable to receive search results from the server");
                    }

                    return false;
                }

                else
                {
                    alert("Unrecognized protocol received from the server.");
                    LOG("ERROR :: Unable to receive search results from the server");

                    return false;
                }
            }

            else
            {
                alert("Corrupt packet received from the server.");
                LOG("ERROR :: Unable to receive search results from the server");

                return false;
            }
        }

        else
        {
            alert("Unable to send friend request protocol to server! Please try again.");
            LOG("ERROR :: Unable to receive search results from the server");

            return false;
        }
    }
}
