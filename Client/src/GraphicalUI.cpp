#include "../include/GraphicalUI.hpp"

namespace prattle
{
    GraphicalUI::GraphicalUI() : m_window{sf::VideoMode{MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT, MIN_WINDOW_BPP}, TITLE, sf::Style::Default},
                                 m_gui{m_window},
                                 m_loginMessage{std::make_shared<tgui::Label>()},
                                 m_signupMessage{std::make_shared<tgui::Label>()},
                                 m_signupDetailsLabel{std::make_shared<tgui::Label>()},
                                 m_connectingText{std::make_shared<tgui::Label>()},
                                 m_menu{std::make_shared<Menu>()},
                                 //m_friendList{std::make_shared<GraphicList>()},
                                 m_connectedUser{std::make_shared<tgui::Label>()},
                                 m_logoutButton{std::make_shared<tgui::Button>()},
                                 m_tabs{std::make_shared<GraphicTab>()},
                                 m_chatBox{std::make_shared<tgui::TextBox>()},
                                 m_inputBox{std::make_shared<tgui::TextBox>()},
                                 m_chatMessage{std::make_shared<tgui::Label>()},
//                                 m_signupScreenButton{std::make_shared<tgui::Button>()},
//                                 m_signupButton{std::make_shared<tgui::Button>()},
//                                 m_connectingText{std::make_shared<tgui::Label>()},
//                                 m_menu{std::make_shared<Menu>()},
//                                 m_connectedUser{std::make_shared<tgui::Label>()},
                                 //m_tabs{std::make_shared<GraphicTab>()},
                                 //m_chatBox{std::make_shared<tgui::TextBox>()},
                                 //m_userInputBox{std::make_shared<tgui::TextBox>()},
                                 //m_state{UserInterface::State::Login}
                                 m_state{UserInterface::State::Login}
    {
        /* Setting the minimum window size for the X window system */
        #if defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD)
            auto win = m_window.getSystemHandle();
            Display* display = XOpenDisplay(NULL);
            XSizeHints* win_size_hints = XAllocSizeHints();
            win_size_hints->flags = PMinSize;
            win_size_hints->min_width = 800;
            win_size_hints->min_height = 600;
//            win_size_hints->max_width = 1200;
//            win_size_hints->max_height = 720;
            XSetWMNormalHints(display, win, win_size_hints);
            XFree(win_size_hints);
            XFlush(display);
            XCloseDisplay(display);
        #endif

        // set the global font of the gui
        m_gui.setGlobalFont(DEFAULT_GLOBAL_FONT);

        // the different panels which act as screens
        m_loginScreen = tgui::Panel::create(sf::Vector2f{MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT});
        m_signupScreen = tgui::Panel::create(sf::Vector2f{MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT});
        m_chatScreen = tgui::Panel::create(sf::Vector2f{MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT});
        m_connectingScreen = tgui::Panel::create(sf::Vector2f{MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT});

        // the common background & logo for the screens
        m_background = tgui::Picture::create(BACKGROUND);
        m_background->setSmooth(true);

        m_logo = tgui::Picture::create(LOGO);
        m_logo->setSmooth(true);
        m_logo->setSize(tgui::bindWidth(m_gui) / 2, tgui::bindWidth(m_gui) / (2 * 1.333333333));
        m_logo->setPosition(tgui::bindRight(m_gui) / 2.2, tgui::bindHeight(m_gui) / 5);

        /* initialization of widgets for the login screen */

        // the login message
        m_loginMessage->setText("Login to start prattling");
        m_loginMessage->setTextSize(22);
        m_loginMessage->setTextColor(sf::Color{70, 66, 66});
        m_loginMessage->setPosition(tgui::bindWidth(m_gui) / 9.5, tgui::bindHeight(m_gui) / 2.25);

        // the username field
        m_usernameField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_usernameField->setDefaultText("Username");
        m_usernameField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 15);
        m_usernameField->setPosition(tgui::bindWidth(m_gui) / 10, tgui::bindHeight(m_gui) / 1.85);

        // the password field
        m_passwordField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_passwordField->setDefaultText("Password");
        m_passwordField->setPasswordCharacter('*');
        m_passwordField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_usernameField));
        m_passwordField->setPosition(tgui::bindWidth(m_gui) / 10, tgui::bindBottom(m_usernameField) + 10);

        // the login button
        m_loginButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_loginButton->setText("Login");
        m_loginButton->setSize(tgui::bindWidth(m_passwordField) - 100, tgui::bindHeight(m_passwordField) - 10);
        m_loginButton->setPosition(tgui::bindLeft(m_usernameField) + m_loginButton->getSize().x / 3.38, tgui::bindBottom(m_passwordField) + 20);

        //std::cout << m_usernameField->getPosition().x - m_loginButton->getPosition().x << " " << (m_usernameField->getPosition().x + m_usernameField->getSize().x) - (m_loginButton->getPosition().x + m_loginButton->getSize().x) << std::endl;

        // the rememebr me checkbox
        m_rememberMeCheckbox = tgui::Checkbox::create();//DEFAULT_TGUI_THEME);
        m_rememberMeCheckbox->setText("Remember me");
        m_rememberMeCheckbox->setSize(tgui::bindWidth(m_passwordField) / 10 - 20, tgui::bindHeight(m_passwordField) - 20);
        m_rememberMeCheckbox->setPosition(tgui::bindLeft(m_passwordField) + 50, tgui::bindBottom(m_loginButton) + 10);
        m_rememberMeCheckbox->uncheck();

        // the signup message displayed for user registration option
        m_signupMessage->setText("New prattler?");
        m_signupMessage->setTextSize(15);
        m_signupMessage->setTextColor(sf::Color{70, 66, 66});
        m_signupMessage->setPosition(tgui::bindLeft(m_usernameField) * m_usernameField->getSize().x * 0.005 + m_signupMessage->getSize().x , tgui::bindHeight(m_gui) / 1.09);
        //std::cout <<  m_signupMessage->getSize().x << std::endl;

        // the button to go to signup screen
        m_signupScreenButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_signupScreenButton->setText("Sign Up!");
        m_signupScreenButton->setTextSize(15);
        m_signupScreenButton->setSize(tgui::bindWidth(m_passwordField) / 2.5, tgui::bindHeight(m_passwordField) - 10);
        m_signupScreenButton->setPosition(tgui::bindRight(m_signupMessage) + 10, tgui::bindHeight(m_gui) / 1.10 - 3);
        m_signupScreenButton->connect("pressed", &GraphicalUI::setState, this, State::Signup);

        // the details label on the signup screen
        m_signupDetailsLabel->setText("Fill your details");
        m_signupDetailsLabel->setTextSize(22);
        m_signupDetailsLabel->setTextColor(sf::Color{70, 66, 66});
        m_signupDetailsLabel->setPosition(tgui::bindWidth(m_gui) / 9, tgui::bindHeight(m_gui) / 2.25);

        m_signupButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_signupButton->setText("Signup");
        m_signupButton->setTextSize(15);
        m_signupButton->setSize(tgui::bindWidth(m_passwordField), tgui::bindHeight(m_passwordField));
        m_signupButton->setPosition(tgui::bindLeft(m_passwordField), tgui::bindHeight(m_gui) / 1.3 - 3);

        m_backButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_backButton->setText("Cancel");
        m_backButton->setTextSize(15);
        m_backButton->setSize(100, 30);
        m_backButton->setPosition(tgui::bindLeft(m_signupButton), tgui::bindHeight(m_gui) / 1.15);
        m_backButton->connect("pressed", &GraphicalUI::setState, this, State::Login);

        m_connectingText->setText("Logging in...");
        m_connectingText->setTextSize(22);
        m_connectingText->setTextColor(sf::Color{70, 66, 66});
        m_connectingText->setPosition(tgui::bindWidth(m_gui) / 5, tgui::bindHeight(m_gui) / 2);

        /* chat screen widgets */

        m_menu->setPosition(tgui::Layout{tgui::bindLeft(m_gui) + 50, tgui::bindTop(m_gui) + 50});

        m_logoutButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_logoutButton->setText("Logout");
        m_logoutButton->setTextSize(15);
        m_logoutButton->setSize(100, 30);
        m_logoutButton->setPosition(tgui::bindRight(m_gui) - m_logoutButton->getSize().x - 20, tgui::bindTop(m_gui) + 50 + 50 / 3);
        //m_logoutButton->connect("pressed", &GraphicalUI::setState, this, State::Login);

        m_connectedUser->setText("Username goes here");
        m_connectedUser->setTextSize(12);
        m_connectedUser->setSize(m_connectedUser->getText().getSize() * 7.2, 15);
        m_connectedUser->setPosition(tgui::bindLeft(m_logoutButton) - m_connectedUser->getSize().x - 10, tgui::bindTop(m_logoutButton) + 15 / 1.6);

        m_tabs->setPosition(tgui::Layout{tgui::bindLeft(m_gui) + 80, tgui::bindTop(m_gui) + 50 + 100});

        m_chatMessage->setText("Select a friend from your friendlist to begin chatting!");
        m_chatMessage->setTextSize(12);
        m_chatMessage->setTextColor(sf::Color{70, 66, 66});
        m_chatMessage->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 2 - 12);

        m_chatBox->setText("");
        m_chatBox->setTextSize(15);
        m_chatBox->setReadOnly(true);
        m_chatBox->setSize(tgui::bindWidth(m_gui) - 110, tgui::bindHeight(m_gui) / 2);
        m_chatBox->setPosition(tgui::bindLeft(m_gui) + 50, tgui::bindBottom(m_logoutButton) + 94);

        m_inputBox->setText("");
        m_inputBox->setTextSize(15);
        m_inputBox->setSize(tgui::bindWidth(m_chatBox), tgui::bindHeight(m_gui) / 8);
        m_inputBox->setPosition(tgui::bindLeft(m_gui) + 50, tgui::bindBottom(m_chatBox) + 10);

        m_chatWindowBorder = tgui::Picture::create(CHAT_WINDOW_BORDER);
        //m_chatWindowBorder.setSize(sf::Vector2f{m_chatBox->getSize().x + 20, m_chatBox->getSize().y + m_inputBox->getSize().y + 10 + 20});
        m_chatWindowBorder->setSize(tgui::bindWidth(m_chatBox) + 20, tgui::bindHeight(m_chatBox) + tgui::bindHeight(m_inputBox) + 30);
        //m_chatWindowBorder.setPosition(sf::Vector2f{m_chatBox->getPosition().x - 10, m_chatBox->getPosition().y - 10});
        m_chatWindowBorder->setPosition(tgui::bindLeft(m_chatBox) - 10, tgui::bindTop(m_chatBox) - 10);

        // add all widgets to the gui
        m_gui.add(m_loginScreen);
        m_gui.add(m_signupScreen);
        m_gui.add(m_chatScreen);
        m_gui.add(m_connectingScreen);

        // add widgets to respective panels

        // The background, logo and caption are common to the login, signup & connecting screeens
        // The background is common to all screens
        m_loginScreen->add(m_background);
        m_loginScreen->add(m_logo);
        m_loginScreen->add(m_loginMessage);
        m_loginScreen->add(m_usernameField);
        m_loginScreen->add(m_passwordField);
        m_loginScreen->add(m_loginButton);
        m_loginScreen->add(m_rememberMeCheckbox);
        m_loginScreen->add(m_signupMessage);
        m_loginScreen->add(m_signupScreenButton);

        m_signupScreen->add(m_background);
        m_signupScreen->add(m_logo);
        m_signupScreen->add(m_signupDetailsLabel);
        m_signupScreen->add(m_signupButton);
        m_signupScreen->add(m_backButton);

        m_connectingScreen->add(m_background);
        m_connectingScreen->add(m_logo);
        m_connectingScreen->add(m_connectingText);

        m_chatScreen->add(m_background);
        m_chatScreen->add(m_menu);
        //m_chatScreen->add(m_friendList);
        m_chatScreen->add(m_connectedUser);
        m_chatScreen->add(m_logoutButton);
        m_chatScreen->add(m_tabs);
        m_chatScreen->add(m_chatMessage);
        m_chatScreen->add(m_chatWindowBorder);
        m_chatScreen->add(m_chatBox);
        m_chatScreen->add(m_inputBox);

        m_loginScreen->hide();
        m_signupScreen->hide();
        m_connectingScreen->hide();
        m_chatScreen->hide();

        setState(m_state);

        reset();
    }

    void GraphicalUI::reset()
    {
        m_usernameField->setText("");
        m_passwordField->setText("");
        m_chatBox->setText("");
        m_inputBox->setText("");

        m_chatBox->moveToFront();
        m_inputBox->moveToFront();
        //m_menu->initList({"Friend 1", "Friend 2", "Friend 3", "Friend 4", "Friend 5", "Friend 6", "Friend 7", "Friend 8", "Friend 9", "Friend 10"});
        m_menu->moveToFront();
    }

    bool GraphicalUI::isMouseOver(tgui::Widget::Ptr widget)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);

        // returns true if the mouse press occured within the bounds of the widget
        return (widget->getPosition().x <= mousePos.x &&
             widget->getPosition().y <= mousePos.y &&
              widget->getPosition().x + widget->getSize().x >= mousePos.x &&
               widget->getPosition().y + widget->getSize().y  >= mousePos.y);
    }

//    bool GraphicalUI::isUIRunning()
//    {
//        return m_window.isOpen();
//    }
//
//    void GraphicalUI::close()
//    {
//        m_window.close();
//    }
//
//    void GraphicalUI::showText()
//    {
//        std::cout << m_usernameField->getText().toAnsiString() << std::endl;
//        std::cout << m_passwordField->getText().toAnsiString() << std::endl;
//    }
//
//    sf::RenderWindow* GraphicalUI::getRenderWindow()
//    {
//        return &m_window;
//    }
//
//    tgui::Gui* GraphicalUI::getGui()
//    {
//        return &m_gui;
//    }

    void GraphicalUI::setState(UserInterface::State s)
    {
        if (m_usernameField->getParent() != nullptr && m_passwordField->getParent() != nullptr)
        {
            m_usernameField->getParent()->remove(m_usernameField);
            m_passwordField->getParent()->remove(m_passwordField);
        }

        switch(s)
        {
            case UserInterface::State::Login:
                {
                    m_loginScreen->add(m_usernameField);
                    m_loginScreen->add(m_passwordField);

                    m_loginScreen->show();
                    m_signupScreen->hide();
                    m_connectingScreen->hide();
                    m_chatScreen->hide();
                }
                break;

            case UserInterface::State::Signup:
                {
                    m_signupScreen->add(m_usernameField);
                    m_signupScreen->add(m_passwordField);

                    m_loginScreen->hide();
                    m_signupScreen->show();
                    m_connectingScreen->hide();
                    m_chatScreen->hide();
                }
                break;

            case UserInterface::State::Connecting:
                {
                    m_loginScreen->hide();
                    m_signupScreen->hide();
                    m_connectingScreen->show();
                    m_chatScreen->hide();
                }
                break;

            case UserInterface::State::Chatting:
                {
                    m_loginScreen->hide();
                    m_signupScreen->hide();
                    m_connectingScreen->hide();
                    m_chatScreen->show();
                }
                break;
        }
    }

    UserInterface::UIEvent GraphicalUI::update()
    {
        if (m_window.isOpen())
        {
            sf::Event event;

            if (m_window.pollEvent(event))
            {
                m_gui.handleEvent(event);

                switch (event.type)
                {
                    case sf::Event::Closed:
                        {
                            m_window.close();
                            return UserInterface::UIEvent::Closed;
                        }
                        break;

                    case sf::Event::Resized:
                        {
                            m_window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                            m_gui.setView(m_window.getView());
                            m_loginScreen->setSize(static_cast<sf::Vector2f>(m_window.getSize()));
                            m_signupScreen->setSize(static_cast<sf::Vector2f>(m_window.getSize()));
                            m_connectingScreen->setSize(static_cast<sf::Vector2f>(m_window.getSize()));
                            m_chatScreen->setSize(static_cast<sf::Vector2f>(m_window.getSize()));
                            m_background->setSize(static_cast<sf::Vector2f>(m_window.getSize()));
                            m_tabs->update();
                        }
                        break;

                    case sf::Event::KeyPressed:
                        {
                            switch (event.key.code)
                            {
                                case sf::Keyboard::Escape:
                                    {
                                        m_window.close();
                                        return UserInterface::UIEvent::Closed;
                                    }
                                    break;
                            }
                        }
                        break;

                    case sf::Event::MouseButtonPressed:
                        {
                            if (isMouseOver(m_loginButton))
                                return UserInterface::UIEvent::UserLogin;

                            if (isMouseOver(m_signupButton))
                                return UserInterface::UIEvent::UserSignup;

                            if (isMouseOver(m_logoutButton))
                                return UserInterface::UIEvent::Disconnect;

                            sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);

                            if (m_menu->getFriendlist()->getBounds().contains(mousePos.x, mousePos.y) && m_menu->getFriendlist()->isVisible() && m_menu->getSelectedFriend() != "")
                            {
                                if (m_tabs->isTabPresent(m_menu->getSelectedFriend()))
                                    m_tabs->focusTab(m_menu->getSelectedFriend());
                                else
                                    m_tabs->addTab(m_menu->getSelectedFriend());

                                m_menu->getFriendlist()->hide();
                            }

                            if (m_menu->getFriendlist()->isVisible() && !(m_menu->getFriendlist()->getBounds().contains(mousePos.x, mousePos.y) || m_menu->getBounds().contains(mousePos.x, mousePos.y)))
                                m_menu->getFriendlist()->hide();
                        }
                        break;
                }
            }

            if (!m_menu->getFriendlist()->getItemCount())
            {
                m_menu->getFriendlist()->setDefaultMessage("No friends yet!");
            }

            if (m_tabs->getTabCount() > 0)
            {
                m_chatMessage->hide();
                m_chatBox->show();
                m_inputBox->show();
                m_chatWindowBorder->show();
            }
            else
            {
                m_chatMessage->show();
                m_chatBox->hide();
                m_inputBox->hide();
                m_chatWindowBorder->hide();
            }

            return UIEvent::None;
        }

        if (!m_window.isOpen())
            return UserInterface::UIEvent::Closed;
    }

    void GraphicalUI::draw()
    {
        m_window.clear();
        m_gui.draw();
        m_window.display();
    }

    void GraphicalUI::alert(const std::string& message)
    {
    }

    std::string GraphicalUI::getInputText()
    {
    }

    std::string GraphicalUI::getUsername()
    {
        return m_usernameField->getText();
    }

    std::string GraphicalUI::getPassword()
    {
        return m_passwordField->getText();
    }

    void GraphicalUI::clearChat()
    {
    }

    std::string GraphicalUI::getChat(const std::string& user)
    {
    }

    void GraphicalUI::addToChatArea(const std::string& text)
    {
    }
}
