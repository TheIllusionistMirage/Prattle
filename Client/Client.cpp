#include "Client.hpp"

namespace chat
{
    void changePanelVisibility(bool &visibility, tgui::Panel::Ptr panel)
    {
        //visibility ? panel->show() : panel->hide();

        if (visibility)
        {
            panel->show();
            visibility = false;
            std::cout << "A";
        }
        else
        {
            panel->hide();
            visibility = true;
            std::cout << "B";
        }
    }

    Client::Client() :
        m_userName("albatross"),
        m_loginStatus(false),
        //m_onlineStatus(chat::OnlineStatus::Unavailable),
        m_width(800),
        m_height(600),
        m_bpp(32),
        m_window(sf::VideoMode(800, 600, 32), "Prattle - v 0.1 [Written by texus, amhndu & TheIllusionistMirage]")//, sf::Style::Closed)
    {
        m_gui.setWindow(m_window);
        m_gui.setGlobalFont(DEFAULT_GLOBAL_FONT);

        m_gui.getGlobalFont();

        std::fstream userLocalInfo(LOCAL_INFO, std::ios::in);

        if (!userLocalInfo.good())
            throw std::runtime_error("ERROR :: Unable to find \"" + LOCAL_INFO + "\" !");

        m_loginPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_loginPanel);

        m_registerPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_registerPanel);

        m_chatPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_chatPanel);

        m_background = tgui::Picture::create(CHAT_BACKGROUND);
        //m_background->setSize(tgui::bindMaximum(800, tgui::bindWidth(m_gui)),tgui::bindMaximum(600, tgui::bindHeight(m_gui)));
        //m_gui.add(m_background);

        m_logo = tgui::Picture::create(DEFAULT_LOGO);
        m_logo->setPosition(tgui::bindWidth(m_gui) / 4, tgui::bindHeight(m_gui) / 4 - 80);
        m_logo->getTooltip()->setText("Always be near");
        //m_gui.add(m_logo);

        m_caption = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_caption->setPosition(tgui::bindWidth(m_gui) / 2, tgui::bindHeight(m_gui) / 3.5);
        m_caption->setText("Always be near");
        m_caption->setTextColor(sf::Color(0, 242, 255));
        //m_gui.add(m_caption);

        m_loginMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_loginMsg->setPosition(tgui::bindWidth(m_gui) / 3 + 30, tgui::bindHeight(m_gui) / 2.25);
        m_loginMsg->setText("Login to start prattling");
        m_loginMsg->setTextSize(22);
        m_loginMsg->setTextColor(sf::Color::White);
        //m_gui.add(m_loginMsg);

        m_usernameField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_usernameField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 15);
        m_usernameField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 1.85);
        m_usernameField->setDefaultText("Username");
        //m_gui.add(m_usernameField, "Username");

        m_passwordField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_passwordField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_usernameField));
        m_passwordField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindBottom(m_usernameField) + 10);
        m_passwordField->setDefaultText("Password");
        m_passwordField->setPasswordCharacter('*');
        //m_gui.add(m_passwordField, "Password");

        m_loginButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_loginButton->setText("Login");
        m_loginButton->setSize(tgui::bindWidth(m_passwordField) - 100, tgui::bindHeight(m_passwordField) - 10);
        m_loginButton->setPosition(tgui::bindWidth(m_gui) / 3 + 100, tgui::bindBottom(m_passwordField) + 20);
        m_loginButton->getTooltip()->setText("Login to start prattling");
        //m_gui.add(m_loginButton);

        m_rememberMeCheckbox = tgui::Checkbox::create(DEFAULT_TGUI_THEME);
        m_rememberMeCheckbox->setText("Remember me");
        m_rememberMeCheckbox->setSize(tgui::bindWidth(m_passwordField) / 10 - 20, tgui::bindHeight(m_passwordField) - 20);
        m_rememberMeCheckbox->setPosition(tgui::bindWidth(m_passwordField) + 100, tgui::bindBottom(m_passwordField) + 60);
        //m_gui.add(m_rememberMeCheckbox);

        m_registerMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_registerMsg->setPosition(tgui::bindWidth(m_gui) / 3.4, tgui::bindHeight(m_gui) / 1.09);
        m_registerMsg->setText("Need an account? Then signup!");
        m_registerMsg->setTextSize(15);
        m_registerMsg->setTextColor(sf::Color::White);
        //m_gui.add(m_registerMsg);

        m_signUpButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_signUpButton->setText("Sign Up");
        m_signUpButton->setTextSize(15);
        m_signUpButton->setSize(tgui::bindWidth(m_passwordField) / 3 + 0, tgui::bindHeight(m_passwordField) - 20);
        m_signUpButton->setPosition(tgui::bindWidth(m_gui) / 1.65, tgui::bindHeight(m_gui) / 1.09 - 3);
        //m_gui.add(m_signUpButton);

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

        m_signUpMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_signUpMsg->setPosition(tgui::bindWidth(m_gui) / 3 + 30, tgui::bindHeight(m_gui) / 2.25);
        m_signUpMsg->setText("Fill your details");
        m_signUpMsg->setTextSize(22);
        m_signUpMsg->setTextColor(sf::Color::White);
        //m_gui.add(m_signUpMsg);

        m_fullNameField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_fullNameField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 15);
        m_fullNameField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 1.85);
        m_fullNameField->setDefaultText("Your full name");
        //m_gui.add(m_fullNameField, "Your full name");

        m_newUsernameField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_newUsernameField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 15);
        m_newUsernameField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 1.85);
        m_newUsernameField->setDefaultText("Pick a username");
        //m_gui.add(m_newUsernameField, "Pick a username");

        m_newPasswordField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_newPasswordField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_usernameField));
        m_newPasswordField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindBottom(m_usernameField) + 10);
        m_newPasswordField->setDefaultText("Choose a password");
        m_newPasswordField->setPasswordCharacter('*');
        //m_gui.add(m_newPasswordField, "Choose a password");
        //m_gui.add(m_newPasswordField);

        m_submitButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_submitButton->setText("Sign Up");
        m_submitButton->setTextSize(15);
        m_submitButton->setSize(tgui::bindWidth(m_passwordField), tgui::bindHeight(m_passwordField) + 10);
        m_submitButton->setPosition(tgui::bindWidth(m_newPasswordField), tgui::bindHeight(m_gui) / 1.3 - 3);
        //m_gui.add(m_submitButton);

        m_backButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_backButton->setText("Back");
        m_backButton->setTextSize(15);
        m_backButton->setSize(100, 30);
        m_backButton->setPosition(tgui::bindWidth(m_newPasswordField) + 80, tgui::bindHeight(m_gui) / 1.15);
        //m_gui.add(m_backButton);

        m_registerPanel->add(m_background);
        m_registerPanel->add(m_logo);
        m_registerPanel->add(m_caption);
        m_registerPanel->add(m_signUpMsg);
        m_registerPanel->add(m_fullNameField);
        m_registerPanel->add(m_newUsernameField);
        m_registerPanel->add(m_newPasswordField);
        m_registerPanel->add(m_submitButton);
        m_registerPanel->add(m_backButton);

////////

        m_userNameLabel = tgui::Label::create();
        m_userNameLabel->setText("Logged in as : " + m_userName);
        m_userNameLabel->setTextSize(15);
        m_userNameLabel->setTextColor(sf::Color::White);
        m_userNameLabel->setPosition(tgui::bindWidth(m_gui) / 1.6 - m_userName.length() * 8, 40);

        m_logoutButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_logoutButton->setText("Logout");
        m_logoutButton->setTextSize(15);
        m_logoutButton->setSize(100, 25);
        m_logoutButton->setPosition(tgui::bindWidth(m_gui) - 150, m_userNameLabel->getPosition().y - 6);

        m_friendListVisibilityButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_friendListVisibilityButton->setText("My Buddies");
        m_friendListVisibilityButton->setTextSize(10);
        m_friendListVisibilityButton->setSize(120, 30);
        m_friendListVisibilityButton->setPosition(40, tgui::bindHeight(m_logoutButton) + 20);

        m_friendChatTabs = tgui::Tab::create();//DEFAULT_TGUI_THEME);
        m_friendChatTabs->setSize(200, 50);
        m_friendChatTabs->setTextSize(12);
        m_friendChatTabs->setTabHeight(20);
        m_friendChatTabs->setPosition(tgui::bindLeft(m_friendListVisibilityButton),
                                      tgui::bindTop(m_friendListVisibilityButton) + m_friendListVisibilityButton->getSize().y + 18);
        m_friendChatTabs->add(frnd1);
        m_friendChatTabs->add(frnd2);
        m_friendChatTabs->add(frnd3);
        m_friendChatTabs->select(0);

        m_friendsOnline = tgui::ListBox::create();
        m_friendsOnline->addItem(frnd1, "rappy");
        m_friendsOnline->addItem(frnd2, "rexy");
        m_friendsOnline->addItem(frnd3, "firey");
        //m_friendsOnline->setSize(100, 100);
        m_friendsOnline->setPosition(10, 10);

        m_chatBox = tgui::TextBox::create();
        m_chatBox->setSize(tgui::bindWidth(m_window) - 80, 320);
        m_chatBox->setTextSize(15);
        m_chatBox->setPosition(40, tgui::bindHeight(m_gui) / 5.2);
        m_chatBox->setReadOnly(true);

        m_inputTextBox = tgui::TextBox::create();//DEFAULT_TGUI_THEME);
        m_inputTextBox->setSize(tgui::bindWidth(m_window) - 80, 110);
        m_inputTextBox->setTextSize(15);
        m_inputTextBox->setPosition(40, tgui::bindHeight(m_window) / 1.35);

        m_friendlistPanel = tgui::Panel::create(sf::Vector2f(200, m_window.getSize().y));
        m_chatPanel->add(m_friendlistPanel);
        m_friendlistPanel->setSize(200, tgui::bindHeight(m_gui) - 100);
        m_friendlistPanel->setPosition(40, tgui::bindHeight(m_friendListVisibilityButton) + 45);
        m_friendlistPanel->add(m_friendsOnline);
        m_friendlistPanel->hide();
        m_friendListVisibilityButton->connect("pressed", changePanelVisibility, panelVisibility, m_friendlistPanel);

        m_chatPanel->add(m_background);
        m_chatPanel->add(m_userNameLabel);
        m_chatPanel->add(m_logoutButton);
        m_chatPanel->add(m_friendChatTabs);
        m_chatPanel->add(m_friendListVisibilityButton);
        //m_chatPanel->add(m_friendsOnline);
        m_chatPanel->add(m_chatBox);
        //m_chatPanel->add(m_friendsOnline);
        m_chatPanel->add(m_inputTextBox);
        m_chatPanel->add(m_friendlistPanel);

////////
        /*m_userNameLabel = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_userNameLabel->setPosition(tgui::bindWidth(m_gui) / 1.6 - m_userName.length() * 10, 40);
        m_userNameLabel->setText("Logged in as " + m_userName);
        m_userNameLabel->setTextSize(15);
        m_userNameLabel->setTextColor(sf::Color::White);

        m_logoutButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_logoutButton->setText("Logout");
        m_logoutButton->setTextSize(15);
        m_logoutButton->setSize(100, 25);
        m_logoutButton->setPosition(tgui::bindWidth(m_gui) - 150, m_userNameLabel->getPosition().y - 6);

        m_chatBox = tgui::TextBox::create();//DEFAULT_TGUI_THEME);
        m_chatBox->setSize(tgui::bindWidth(m_window) - 80, 320);
        m_chatBox->setTextSize(15);
        m_chatBox->setPosition(40, tgui::bindHeight(m_gui) / 6);
        m_chatBox->setReadOnly(true);

        m_userInputTextbox = tgui::TextBox::create();//DEFAULT_TGUI_THEME);
        m_userInputTextbox->setSize(tgui::bindWidth(m_window) - 80, 120);
        m_userInputTextbox->setTextSize(15);
        m_userInputTextbox->setPosition(40, tgui::bindHeight(m_window) / 1.4);

        m_friendListButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_friendListButton->setPosition();

        m_chatTab = tgui::Tab::create(DEFAULT_TGUI_THEME);
        m_chatTab->setSize(70, 20);
        m_chatTab->setTextSize(10);
        m_chatTab->setTabHeight(20);
        m_chatTab->setPosition(40, 70);
        m_chatTab->add("Friend 1");
        m_chatTab->add("Friend 2");
        m_chatTab->add("Friend 3");
        m_chatTab->select(0);

        m_chatPanel->add(m_background);
        //m_chatPanel->add(m_logo);
        //m_chatPanel->add(m_caption);
        m_chatPanel->add(m_chatTab);
        m_chatPanel->add(m_userNameLabel);
        m_chatPanel->add(m_logoutButton);
        m_chatPanel->add(m_chatBox);
        m_chatPanel->add(m_userInputTextbox);*/

        /*
        m_gui.add(m_userNameLabel);
        m_gui.add(m_logoutButton);
        m_gui.add(m_chatTextbox);
        m_gui.add(m_sendMsgButton);*/
/////////////
        m_loginPanel->hide();
        m_registerPanel->hide();
        m_chatPanel->hide();

        m_loginButton->connect("pressed", &Client::changeScreenState, this, ScreenState::ChatScreen);
        m_signUpButton->connect("pressed", &Client::changeScreenState, this, ScreenState::SignUpScreen);
        m_backButton->connect("pressed", &Client::changeScreenState, this, ScreenState::LoginScreen);

        m_screenState = chat::ScreenState::LoginScreen;
        //m_screenState = chat::ScreenState::SignUpScreen;
    }

    void Client::changeScreenState(chat::ScreenState screenState)
    {
        m_screenState = screenState;
    }

    //void Client::changePanelVisibility(tgui::Panel::Ptr panel)

    /*void Client::loginPrompt()
    {
        std::cout << "          ==========================" << std::endl;
        std::cout << "          |  CHAT PROGRAM - v 0.1  |" << std::endl;
        std::cout << "          ==========================" << std::endl << std::endl;

        std::cout << "By texus, amhndu & TheIllusionistMirage" << std::endl << std::endl;

        char choice;

        do
        {
            std::cout << "(R)egister or (L)ogin to start chatting!" << std::endl;
            std::cin >> choice;
            std::cin.ignore();

            switch (tolower(choice))
            {
                case 'r' : signup(); break;

                case 'l' : login(); break;

                default  : std::cout << "Please enter a valid option!" << std::endl;
            }

        } while (choice != 'r' && choice != 'l');
    }*/

    /*void Client::signup()
    {
        sf::Socket::Status connectionSuccess = m_client.connect(chat::SERVER_IP_ADDRESS, chat::OPEN_PORT);

        if (connectionSuccess == sf::Socket::Done)
        {
            std::string info = "new_user";

            std::cout << "Pick a username : ";
            std::getline(std::cin, m_userName);

            std::cout << "Choose a password [Tip : Make you passwor strong by including lowercase/uppercase letters, digits and special characters] : ";
            std::getline(std::cin, m_password);

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
    }*/

    /*bool Client::login()
    {
        m_screenState = chat::ScreenState::LoginScreen;

        sf::Socket::Status connectionSuccess = m_client.connect(chat::SERVER_IP_ADDRESS, chat::OPEN_PORT);

        if (connectionSuccess == sf::Socket::Done)
        {
            std::string info = "existing_user";

            std::cout << "Enter your username : ";
            std::getline(std::cin, m_userName);
            std::cout << "Enter your password : ";
            std::getline(std::cin, m_password);

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
                            std::cout << "Login Successful!" << std::endl;

                            std::cout << "Enter the name of the person you want to chat with : ";
                            std::getline(std::cin, m_friend);
                            m_onlineStatus = chat::OnlineStatus::Available;
                            m_client.setBlocking(false);

                            return true;
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
        return false;

    }*/

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
                std::cout << sender << " : " << data << std::endl;
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

    const std::string& Client::getUserName()
    {
        return m_userName;
    }

    const std::string& Client::getFriendName()
    {
        return m_friend;
    }

    void Client::logout()
    {
        m_client.disconnect();
        m_loginStatus = false;
    }

    /*void Client::drawLoginScreen()
    {

    }

    void Client::drawRegistrationScreen()
    {

    }

    void Client::drawChatScreen()
    {

    }

    void Client::updateScreen()
    {
        if (m_window.isOpen())
        {
            sf::Event event;
        }
    }*/

    void Client::renderScreen()
    {
        switch (m_screenState)
        {
            case chat::ScreenState::LoginScreen:
                {
                    //m_registerPanel->show();
                    //m_loginPanel->hide();
                    //std::cout << (int)m_screenState << std::endl;

                    m_loginPanel->show();
                    m_registerPanel->hide();
                    m_chatPanel->hide();
                } break;

            case chat::ScreenState::SignUpScreen:
                {
                    //m_loginPanel->show();
                    //m_registerPanel->hide();
                    //std::cout << (int)m_screenState << std::endl;

                    m_registerPanel->show();
                    m_loginPanel->hide();
                    m_chatPanel->hide();
                } break;

            case chat::ScreenState::ChatScreen:
                {
                    m_chatPanel->show();
                    m_loginPanel->hide();
                    m_registerPanel->hide();

                    /*auto chatBoxSize = m_chatBox->getSize();
                    auto chatBoxPos = m_chatBox->getPosition();
                    if (m_friendlistPanel->isVisible())
                    {
                        m_chatBox->setSize(m_chatBox->getSize().x - m_friendlistPanel->getSize().x - 10, m_chatBox->getPosition().y);
                        m_chatBox->setPosition(m_friendlistPanel->getPosition().x + m_friendlistPanel->getSize().x + 10, m_chatBox->getPosition().y);
                    }
                    else
                    {
                        m_chatBox->setSize(chatBoxSize);
                        m_chatBox->setPosition(chatBoxPos);
                    }*/
                } break;
        }

        m_window.clear(DEFAULT_BG_COLOR);
        m_gui.draw();
        m_window.display();
    }

    void Client::run()
    {
        while (m_window.isOpen())
        {
            sf::Event event;

            while (m_window.pollEvent(event))
            {
                switch (event.type)
                {
                    case sf::Event::Closed:
                        {
                            m_window.close();
                            logout();
                        } break;

                    /*case sf::Event::Resized:
                        {
                            m_window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                            m_gui.setView(m_window.getView());
                        } break;*/
                }

                m_gui.handleEvent(event);
            }

            renderScreen();
        }
    }
}
