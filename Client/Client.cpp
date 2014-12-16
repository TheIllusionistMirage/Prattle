#include "Client.hpp"

namespace chat
{
    Client::Client()
    {
        m_loginStatus = false;
        m_onlineStatus = Status::Offline;

        m_width = 800;
        m_height = 600;
        m_bpp = 32;
        m_window.create(sf::VideoMode(m_width, m_height, m_bpp), "Prattle - v 0.1 [Written by texus, amhndu & TheIllusionistMirage]");

        /* Initializing the GUI */

        // Initialize the SFML window as the renderer for the TGUI widgets
        m_gui.setWindow(m_window);
        // Set the global font for TGUI
        m_gui.setGlobalFont(DEFAULT_GLOBAL_FONT);

        // Initialize the panels which serve as the login screen, the signup
        // screen and the chat screen. Add them to TGUI
        m_loginPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_loginPanel);

        m_registerPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_registerPanel);

        m_chatPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_chatPanel);

        // The background image of the application
        m_background = tgui::Picture::create(CHAT_BACKGROUND);

        // The logo of the application
        m_logo = tgui::Picture::create(DEFAULT_LOGO);
        m_logo->setPosition(tgui::bindWidth(m_gui) / 4, tgui::bindHeight(m_gui) / 4 - 80);
        m_logo->getTooltip()->setText("Always be near");

        // The motto of Prattle (Always be near)
        m_caption = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_loginMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_usernameField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_passwordField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_loginButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_rememberMeCheckbox = tgui::Checkbox::create(DEFAULT_TGUI_THEME);
        m_registerMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_signUpButton = tgui::Button::create(DEFAULT_TGUI_THEME);

        // Add all these widgets to the login panel so that they get
        // displayed in the login screen
        m_loginPanel->add(m_background);
        m_loginPanel->add(m_logo);
        m_loginPanel->add(m_caption);
        m_loginPanel->add(m_loginMsg);
        m_loginPanel->add(m_usernameField);
        m_loginPanel->add(m_passwordField);
        m_loginPanel->add(m_loginButton);
        m_loginPanel->add(m_rememberMeCheckbox);
        m_loginPanel->add(m_registerMsg);
        m_loginPanel->add(m_signUpButton);

        // Widgets for signup screen
        m_signUpMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_fullNameField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_newUsernameField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_newPasswordField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_submitButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_backButton = tgui::Button::create(DEFAULT_TGUI_THEME);

        // Add all these widgets to the signup panel so they get
        // displayed in the signup screen
        m_registerPanel->add(m_background);
        m_registerPanel->add(m_logo);
        m_registerPanel->add(m_caption);
        m_registerPanel->add(m_signUpMsg);
        m_registerPanel->add(m_fullNameField);
        m_registerPanel->add(m_newUsernameField);
        m_registerPanel->add(m_newPasswordField);
        m_registerPanel->add(m_submitButton);
        m_registerPanel->add(m_backButton);

        // Chat screen related widgets
        m_userNameLabel = tgui::Label::create();
        m_logoutButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_friendListVisibilityButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_friendsOnline = tgui::ListBox::create(DEFAULT_TGUI_THEME);
        m_friendChatTabs = tgui::Tab::create();
        m_chatBox = tgui::TextBox::create();
        m_inputTextBox = tgui::TextBox::create();
        m_friendlistPanel = tgui::Panel::create(sf::Vector2f(200, m_window.getSize().y));

        m_friendListVisibilityButton->connect("pressed", &Client::changePanelVisibility, this, panelVisibility, m_friendlistPanel);

        // Add these widgets to the chat panel
        m_chatPanel->add(m_background);
        m_chatPanel->add(m_userNameLabel);
        m_chatPanel->add(m_logoutButton);
        m_chatPanel->add(m_friendChatTabs);
        m_chatPanel->add(m_friendListVisibilityButton);
        m_chatPanel->add(m_chatBox);
        m_chatPanel->add(m_inputTextBox);
        m_chatPanel->add(m_friendlistPanel);

        m_loginPanel->hide();
        m_registerPanel->hide();
        m_chatPanel->hide();

        m_loginButton->connect("pressed", &Client::login, this);
        m_signUpButton->connect("pressed", &Client::changeScreenState, this, ScreenState::SignupScreen);
        m_backButton->connect("pressed", &Client::changeScreenState, this, ScreenState::LoginScreen);
        m_logoutButton->connect("pressed", &Client::changeScreenState, this, ScreenState::LoginScreen);

        m_screenState = chat::ScreenState::LoginScreen;
        changeScreenState(m_screenState);

        m_messageWindow = tgui::ChildWindow::create();
        m_messageLabel = tgui::Label::create();

        m_messageWindow->connect("Closed", std::bind(&tgui::ChildWindow::hide, m_messageWindow));
    }

    void Client::init()
    {
        m_loginStatus = false;
        m_onlineStatus = Status::Offline;

        /** The following for setting minimum window size is applicable only for X based window systems **/
        /* Setting the minimum window size for the X window system */
        auto win = m_window.getSystemHandle();
        Display* display = XOpenDisplay(NULL);
        XSizeHints* win_size_hints = XAllocSizeHints();
        win_size_hints->flags = PMinSize;
        win_size_hints->min_width = 800;
        win_size_hints->min_height = 600;
        XSetWMNormalHints(display, win, win_size_hints);
        XFree(win_size_hints);
        XFlush(display);
        XCloseDisplay(display);

        m_caption->setPosition(tgui::bindWidth(m_gui) / 2, tgui::bindHeight(m_gui) / 3.5);
        m_caption->setText("Always be near");
        m_caption->setTextColor(sf::Color(0, 242, 255));

        m_loginMsg->setPosition(tgui::bindWidth(m_gui) / 3 + 30, tgui::bindHeight(m_gui) / 2.25);
        m_loginMsg->setText("Login to start prattling");
        m_loginMsg->setTextSize(22);
        m_loginMsg->setTextColor(sf::Color::White);

        m_usernameField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 15);
        m_usernameField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 1.85);
        m_usernameField->setDefaultText("Username");

        m_passwordField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_usernameField));
        m_passwordField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindBottom(m_usernameField) + 10);
        m_passwordField->setDefaultText("Password");
        m_passwordField->setPasswordCharacter('*');

        m_loginButton->setText("Login");
        m_loginButton->setSize(tgui::bindWidth(m_passwordField) - 100, tgui::bindHeight(m_passwordField) - 10);
        m_loginButton->setPosition(tgui::bindWidth(m_gui) / 3 + 100, tgui::bindBottom(m_passwordField) + 20);
        m_loginButton->getTooltip()->setText("Login to start prattling");

        m_rememberMeCheckbox->setText("Remember me");
        m_rememberMeCheckbox->setSize(tgui::bindWidth(m_passwordField) / 10 - 20, tgui::bindHeight(m_passwordField) - 20);
        m_rememberMeCheckbox->setPosition(tgui::bindWidth(m_passwordField) + 100, tgui::bindBottom(m_passwordField) + 60);

        m_registerMsg->setPosition(tgui::bindWidth(m_gui) / 3.4, tgui::bindHeight(m_gui) / 1.09);
        m_registerMsg->setText("Need an account? Then signup!");
        m_registerMsg->setTextSize(15);
        m_registerMsg->setTextColor(sf::Color::White);

        m_signUpButton->setText("Sign Up");
        m_signUpButton->setTextSize(15);
        m_signUpButton->setSize(tgui::bindWidth(m_passwordField) / 3 + 0, tgui::bindHeight(m_passwordField) - 20);
        m_signUpButton->setPosition(tgui::bindWidth(m_gui) / 1.65, tgui::bindHeight(m_gui) / 1.09 - 3);

        // Add all these widgets to the login panel so that they get
        // displayed in the login screen
        m_loginPanel->add(m_background);
        m_loginPanel->add(m_logo);
        m_loginPanel->add(m_caption);
        m_loginPanel->add(m_loginMsg);
        m_loginPanel->add(m_usernameField);
        m_loginPanel->add(m_passwordField);
        m_loginPanel->add(m_loginButton);
        m_loginPanel->add(m_rememberMeCheckbox);
        m_loginPanel->add(m_registerMsg);
        m_loginPanel->add(m_signUpButton);

        // Widgets for signup screen

        m_signUpMsg->setPosition(tgui::bindWidth(m_gui) / 3 + 30, tgui::bindHeight(m_gui) / 2.25);
        m_signUpMsg->setText("Fill your details");
        m_signUpMsg->setTextSize(22);
        m_signUpMsg->setTextColor(sf::Color::White);

        m_fullNameField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 15);
        m_fullNameField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 1.85);
        m_fullNameField->setDefaultText("Your full name");

        m_newUsernameField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 15);
        m_newUsernameField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 1.85);
        m_newUsernameField->setDefaultText("Pick a username");

        m_newPasswordField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_usernameField));
        m_newPasswordField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindBottom(m_usernameField) + 10);
        m_newPasswordField->setDefaultText("Choose a password");
        m_newPasswordField->setPasswordCharacter('?');

        m_submitButton->setText("Sign Up");
        m_submitButton->setTextSize(15);
        m_submitButton->setSize(tgui::bindWidth(m_passwordField), tgui::bindHeight(m_passwordField) + 10);
        m_submitButton->setPosition(tgui::bindWidth(m_newPasswordField), tgui::bindHeight(m_gui) / 1.3 - 3);

        m_backButton->setText("Back");
        m_backButton->setTextSize(15);
        m_backButton->setSize(100, 30);
        m_backButton->setPosition(tgui::bindWidth(m_newPasswordField) + 80, tgui::bindHeight(m_gui) / 1.15);

        m_userNameLabel->setText("Logged in as : " + m_userName);
        m_userNameLabel->setTextSize(15);
        m_userNameLabel->setTextColor(sf::Color::White);
        m_userNameLabel->setPosition(m_window.getSize().x - 100 - (40 + 100 + 20) - m_userName.length() * 10, 40);

        m_logoutButton->setText("Logout");
        m_logoutButton->setTextSize(15);
        m_logoutButton->setSize(100, 25);
        m_logoutButton->setPosition(tgui::bindWidth(m_gui) - 150, m_userNameLabel->getPosition().y - 6);

        m_friendListVisibilityButton->setText("My Buddies");
        m_friendListVisibilityButton->setTextSize(12);
        m_friendListVisibilityButton->setSize(120, 30);
        m_friendListVisibilityButton->setPosition(40, tgui::bindHeight(m_logoutButton) + 20);

        m_friendsOnline->setPosition(10, 10);

        m_friendChatTabs->setSize(200, 50);
        m_friendChatTabs->setTextSize(12);
        m_friendChatTabs->setTabHeight(20);
        m_friendChatTabs->setPosition(tgui::bindLeft(m_friendListVisibilityButton), tgui::bindTop(m_friendListVisibilityButton) + m_friendListVisibilityButton->getSize().y + 18);

        m_chatBox->setSize(tgui::bindWidth(m_window) - 80, 320);
        m_chatBox->setTextSize(15);
        m_chatBox->setPosition(40, tgui::bindHeight(m_gui) / 5.2);
        m_chatBox->setReadOnly(true);

        m_inputTextBox->setSize(tgui::bindWidth(m_window) - 80, 110);
        m_inputTextBox->setTextSize(15);
        m_inputTextBox->setPosition(40, tgui::bindHeight(m_window) / 1.35);

        m_friendlistPanel->setSize(tgui::bindWidth(m_friendsOnline) + 20, tgui::bindHeight(m_friendsOnline) + 20);
        m_friendlistPanel->setPosition(40, tgui::bindHeight(m_friendListVisibilityButton) + 45);
        m_friendlistPanel->add(m_friendsOnline);
        m_friendlistPanel->hide();
        m_friendListVisibilityButton->connect("pressed", &Client::changePanelVisibility, this, panelVisibility, m_friendlistPanel);

        m_gui.add(m_messageWindow, "message_window");
        m_messageWindow->hide();
        m_messageWindow->keepInParent(true);
        m_messageWindow->add(m_messageLabel);
    }

    bool Client::checkIfWhitespace(const std::string& message)
    {
        for (auto itr = message.begin(); itr != message.end(); itr++)
        {
            if (!tgui::isWhitespace(*itr))
            {
                return false;
            }
        }

        return true;
    }

    void Client::changePanelVisibility(bool &visibility, tgui::Panel::Ptr panel)
    {
        if (visibility)
        {
            panel->show();
            visibility = false;
        }
        else
        {
            panel->hide();
            visibility = true;
        }
    }

    void Client::changeScreenState(const ScreenState& screenState)
    {
        m_screenState = screenState;

        selectScreenForRendering();
    }

    void Client::selectScreenForRendering()
    {
        switch (m_screenState)
        {
            case chat::ScreenState::LoginScreen:
                {
                    logout();

                    if (!m_rememberMeCheckbox->isChecked())
                    {
                        m_usernameField->setText("");
                        m_passwordField->setText("");
                    }
                    m_loginPanel->show();
                    m_registerPanel->hide();
                    m_chatPanel->hide();
                } break;

            case chat::ScreenState::SignupScreen:
                {
                    m_loginPanel->hide();
                    m_registerPanel->show();
                    m_chatPanel->hide();
                } break;

            case chat::ScreenState::ChatScreen:
                {
                    m_loginPanel->hide();
                    m_registerPanel->hide();
                    m_chatPanel->show();
                } break;
        }
    }

    void Client::run()
    {
        std::string message;
        while (m_window.isOpen())
        {
            sf::Event event;
            while (m_window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    m_window.close();

                if (event.type == sf::Event::Resized)
                {
                    m_window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                    m_gui.setView(m_window.getView());
                }

                if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
                {
                    if (isLoggedIn())
                        if (!checkIfWhitespace(m_inputTextBox->getText()))
                        {
                            message = m_inputTextBox->getText();
                            m_chatBox->addText(m_userName + " : " + message);
                            m_inputTextBox->setText("");

                            sf::Packet msgPacket;
                            msgPacket << m_userName << m_friends.back() << message;

                            //if (sf::Event::KeyReleased)
                                if (send(msgPacket) == sf::Socket::Error)
                                    std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Error in sending message! Please try again" << std::endl;
                        }
                }

                m_gui.handleEvent(event);
            }

            if (isLoggedIn())
            {
                receive();
            }

            m_window.clear(sf::Color(255, 158, 54));
            m_gui.draw();
            m_window.display();
        }
    }

    void Client::signup()
    {
        changeScreenState(ScreenState::SignupScreen);

        sf::Socket::Status connectionSuccess = m_client.connect(chat::SERVER_IP_ADDRESS, chat::OPEN_PORT);

        if (connectionSuccess == sf::Socket::Done)
        {
            std::string info = "new_user";

            /*std::cout << "Pick a username : ";
            std::getline(std::cin, m_userName);

            std::cout << "Choose a password [Tip : Make you passwor strong by including lowercase/uppercase letters, digits and special characters] : ";
            std::getline(std::cin, m_password);*/

            m_userName = m_newUsernameField->getText();
            m_password = m_passwordField->getText();

            sf::Packet msgPacket;
            msgPacket << m_userName << m_password << info;

            if (m_client.send(msgPacket) == sf::Socket::Done)
            {
                sf::Socket::Status status = m_client.receive(msgPacket);

                if (status == sf::Socket::Done)
                {
                    std::string serverReply;

                    if (msgPacket >> serverReply)
                    {
                        if (serverReply == "registered")
                        {
                            m_loginStatus = false;
                            std::cout << "Registration successful!" << std::endl;
                            std::cout << "Now login to start chatting!" << std::endl;
                        }

                        else if (serverReply != "registered")
                        {
                            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Problem occured in registration! Please try again" << std::endl;
                        }
                    }
                }
            }
            else if (m_client.send(msgPacket) == sf::Socket::Error)
                std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: An error occured in registration! Please try again" << std::endl;
        }
        /*else
        {
            tgui::ChildWindow::Ptr errorWindow = tgui::ChildWindow::create();
            errorWindow->setSize(200, 50);
            errorWindow->setTitle("Error!");
            m_gui.add(errorWindow);

            tgui::Label::Ptr errorLabel = tgui::Label::create();
            errorLabel->setText("Unable to connect to server! Please make sure that the server is up and you're connected to the internet.");
            errorLabel->setTextSize(12);
            errorLabel->setPosition(20, 10);
            errorWindow->add(errorLabel);
        }*/
    }

    bool Client::login()
    {
        logout();

        sf::Socket::Status connectionSuccess = m_client.connect(chat::SERVER_IP_ADDRESS, chat::OPEN_PORT);

        if (connectionSuccess == sf::Socket::Done)
        {
            std::string info = "existing_user";

            if (m_usernameField->getText() != "" && m_passwordField->getText() != "")
            {
                m_userName = m_usernameField->getText();
                m_password = m_passwordField->getText();

                sf::Packet msgPacket;
                msgPacket << m_userName << m_password << info;

                if (m_client.send(msgPacket) == sf::Socket::Done)
                {
                    sf::Socket::Status status = m_client.receive(msgPacket);

                    if (status == sf::Socket::Done)
                    {
                        std::string serverReply;

                        if (msgPacket >> serverReply)
                        {
                            if (serverReply == "registered")
                            {
                                m_loginStatus = true;
                                m_userNameLabel->setText( "Logged in as : " + m_userName);

                                m_userNameLabel->setPosition(m_window.getSize().x - 100 - (40 + 100 + 20) - m_userName.length() * 10, 40);

                                std::cout << "Enter the name of the person you want to chat with : ";
                                std::string name;
                                std::getline(std::cin, name);
                                m_friends.empty();
                                m_friends.push_back(name);
                                m_friendsOnline->removeAllItems();
                                m_friendsOnline->addItem(name, "frnd1");
                                m_friendChatTabs->add(name);
                                m_friendChatTabs->select(0);
                                m_client.setBlocking(false);
                                changeScreenState(ScreenState::ChatScreen);


                            }

                            else if(serverReply == "unregistered")
                            {
                                m_messageWindow->setSize(400, 60);
                                m_messageWindow->setTitle("Error!");
                                m_messageWindow->setPosition(tgui::bindWidth(m_gui) / 2 - 200, tgui::bindHeight(m_gui) / 2 - m_messageLabel->getSize().y);
                                m_messageLabel->setText("Can't recognize the username-password combination!\nIf you're not registered with us, please consider signing up.");
                                m_messageLabel->setTextSize(12);
                                m_messageLabel->setPosition(20, 10);

                                if (!m_messageWindow->isVisible())
                                    m_messageWindow->show();
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
                m_messageWindow->setSize(400, 60);
                m_messageWindow->setTitle("Error!");
                m_messageWindow->setPosition(tgui::bindWidth(m_gui) / 2 - 200, tgui::bindHeight(m_gui) / 2 - m_messageWindow->getSize().y);
                m_messageLabel->setText("You can't leave either of the fields blank!");
                m_messageLabel->setTextSize(12);
                m_messageLabel->setPosition(20, 10);

                if (!m_messageWindow->isVisible())
                    m_messageWindow->show();
            }
        }
        else
        {
            m_messageWindow->setSize(400, 60);
            m_messageWindow->setTitle("Error!");
            m_messageWindow->setPosition(tgui::bindWidth(m_gui) / 2 - 200, tgui::bindHeight(m_gui) / 2 - m_messageLabel->getSize().y);
            m_messageLabel->setText("Unable to connect to server! Please make sure that the\n     server is up and you're connected to the internet.");
            m_messageLabel->setTextSize(12);
            m_messageLabel->setPosition(20, 10);

            if (!m_messageWindow->isVisible())
                m_messageWindow->show();
        }
    }

    bool Client::isLoggedIn()
    {
        return m_loginStatus;
    }

    sf::Socket::Status Client::send(sf::Packet& packet)
    {
        return m_client.send(packet);
    }

    bool Client::receive()
    {
        sf::Packet dataPacket;
        sf::Socket::Status status = m_client.receive(dataPacket);

        while(status == sf::Socket::Done)
        {
            std::string sender;
            std::string data;

            if (dataPacket >> sender >> data)
            {
                //std::cout << sender << " : " << data << std::endl;

                if (data != "")
                    m_chatBox->addText(sender + " : " + data);
            }

            status = m_client.receive(dataPacket);
        }

        if (status == sf::Socket::Error)
        {
            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Unable to receive data from remote peer!" <<  std::endl;
            return false;
        }

        return true;
    }

    //const std::string& Client::getUserName()
    //{
    //    return m_userName;
    //}

    //const std::string& Client::getFriendName()
    //{
    //    return m_friend;
    //}

    bool Client::logout()
    {
        m_client.disconnect();
        m_client.setBlocking(true);
        init();
    }
}
