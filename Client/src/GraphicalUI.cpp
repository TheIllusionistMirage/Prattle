#include "../include/GraphicalUI.hpp"
#include "../include/Logger.hpp"

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
                                // m_alertBox{std::make_shared<tgui::ChildWindow>()},
                                 m_alertMessage{std::make_shared<tgui::Label>()},
//                                 m_signupScreenButton{std::make_shared<tgui::Button>()},
//                                 m_signupButton{std::make_shared<tgui::Button>()},
//                                 m_connectingText{std::make_shared<tgui::Label>()},
//                                 m_menu{std::make_shared<Menu>()},
//                                 m_connectedUser{std::make_shared<tgui::Label>()},
                                 //m_tabs{std::make_shared<GraphicTab>()},
                                 //m_chatBox{std::make_shared<tgui::TextBox>()},
                                 //m_userInputBox{std::make_shared<tgui::TextBox>()},
                                 //m_state{UserInterface::State::Login}
                                 //m_searchBox{std::make_shared<tgui::ChildWindow>()},
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

        m_alertMessage->setTextColor(sf::Color::White);

        m_alertBox = tgui::ChildWindow::create(DEFAULT_TGUI_THEME);
        m_alertBox->setTitle("Alert");
        m_alertBox->hide();
        m_alertBox->add(m_alertMessage);
        m_alertBox->connect("Closed", &GraphicalUI::closeAlert, this);
        m_gui.add(m_alertBox);

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
        m_loginButton->connect("pressed", &GraphicalUI::getUIEvent, this, "login");

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
        //m_signupScreenButton->connect("pressed", &GraphicalUI::setState, this, State::Signup);

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
        //m_backButton->connect("pressed", &GraphicalUI::setState, this, State::Login);

        m_connectingText->setText("Connecting...");
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
        m_logoutButton->connect("pressed", &GraphicalUI::getUIEvent, this, "logout");

        m_connectedUser->setText("Username goes here");
        m_connectedUser->setTextSize(12);
        m_connectedUser->setSize(m_connectedUser->getText().getSize() * 3, 15);
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
        m_state = State::Login;;
        setState(m_state);

        //m_usernameField->setText("");
        m_passwordField->setText("");
        m_chatBox->setText("");
        m_inputBox->setText("");

        m_chatBox->moveToFront();
        m_inputBox->moveToFront();
        m_menu->resetList();
        m_menu->moveToFront();
        m_tabs->clear();
    }

    bool GraphicalUI::isMouseOver(tgui::Widget::Ptr widget)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);

        // returns true if the mouse press occured within the bounds of the tgui element
        return (widget->getAbsolutePosition().x <= mousePos.x &&
             widget->getAbsolutePosition().y <= mousePos.y &&
              widget->getAbsolutePosition().x + widget->getSize().x >= mousePos.x &&
               widget->getAbsolutePosition().y + widget->getSize().y  >= mousePos.y);
    }
//
//    bool GraphicalUI::isMouseOver(tgui::Container::Ptr panel)
//    {
//        sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
//
//        // returns true if the mouse press occured within the bounds of the panel
//
//        return (panel->getPosition().x <= mousePos.x &&
//             panel->getPosition().y <= mousePos.y &&
//              panel->getPosition().x + panel->getSize().x >= mousePos.x &&
//               panel->getPosition().y + panel->getSize().y  >= mousePos.y);
//    }

    void GraphicalUI::setState(UserInterface::State s)
    {
        m_state = s;

        if (m_usernameField->getParent() != nullptr && m_passwordField->getParent() != nullptr)
        {
            m_usernameField->getParent()->remove(m_usernameField);
            m_passwordField->getParent()->remove(m_passwordField);
        }

        switch(m_state)
        {
            case UserInterface::State::Login:
                m_loginScreen->add(m_usernameField);
                m_loginScreen->add(m_passwordField);

                m_loginScreen->show();
                m_signupScreen->hide();
                m_connectingScreen->hide();
                m_chatScreen->hide();
                break;
            case UserInterface::State::Signup:
                m_signupScreen->add(m_usernameField);
                m_signupScreen->add(m_passwordField);

                m_loginScreen->hide();
                m_signupScreen->show();
                m_connectingScreen->hide();
                m_chatScreen->hide();
                break;
            case UserInterface::State::Connecting:
                m_loginScreen->hide();
                m_signupScreen->hide();
                m_connectingScreen->show();
                m_chatScreen->hide();
                break;
            case UserInterface::State::Chatting:
                m_loginScreen->hide();
                m_signupScreen->hide();
                m_connectingScreen->hide();
                m_chatScreen->show();
                m_connectedUser->setText(m_usernameField->getText());
                break;
            case UserInterface::State::Exit:
                break;
        }
    }

    UserInterface::State GraphicalUI::getState()
    {
        return m_state;
    }

    UserInterface::UIEvent GraphicalUI::update()
    {
        if (m_window.isOpen())
        {
            sf::Event event;

            while (m_window.pollEvent(event))
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
                                case sf::Keyboard::F4:
                                    {
                                        if (event.key.alt)
                                            return UserInterface::UIEvent::Closed;
                                    }
                                    break;

                                case sf::Keyboard::Return:
                                    {
                                        // fill username/password and hit enter
                                        // instead of having to reach for the
                                        // mouse to click the login button
                                        if (m_state == State::Login &&
                                             !isStringWhitespace(getUsername()) &&
                                              !isStringWhitespace(getPassword()))
                                        {
                                            return UIEvent::UserLogin;
                                        }
                                        else if (m_state == State::Signup &&
                                             !isStringWhitespace(getUsername()) &&
                                              !isStringWhitespace(getPassword()))
                                        {
                                            DBG_LOG("Signup triggered by pressing return.");
                                            return UIEvent::UserSignup;
                                        }
                                        else if (m_state == State::Chatting)
                                        {
                                            if (event.key.shift && m_inputBox->isFocused())
                                                m_inputBox->addText("\n");
                                            if (!isStringWhitespace(getInputText()) && m_inputBox->isFocused())
                                            {
                                                //setInputText(getInputText().substr(0, getInputText().length() - 1));
                                                DBG_LOG("Attempt to send message");
                                                return UIEvent::MessageSent;
                                            }
                                        }
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case sf::Event::MouseButtonPressed:
                        {
                            // if alertbox is visible, all other widgets freeze
                            // and are inactive. So don't update them.
                            if (!m_alertBox->isVisible())
                            {
                                // check if mouse pointer is over login, signup
                                // or logout buttons. TGUI already allows signals
                                // to handle button press events but doing the
                                // following is easier and less code.
                                if (isMouseOver(m_loginButton) && getState() == State::Login)
                                    return UserInterface::UIEvent::UserLogin;

                                if (isMouseOver(m_signupScreenButton) && getState() == State::Login)
                                {
                                    setState(State::Signup);
                                    return UserInterface::UIEvent::StateChanged;
                                }

                                if (isMouseOver(m_signupButton) && getState() == State::Signup)
                                {
                                    DBG_LOG("Signup triggered from clicking the signup button.");
                                    return UserInterface::UIEvent::UserSignup;
                                }

                                if (isMouseOver(m_logoutButton) && getState() == State::Chatting)
                                    return UserInterface::UIEvent::Disconnect;

                                auto sp = m_menu->getMenuItem(Menu::Item::SearchPanel);

                                auto c = std::static_pointer_cast<tgui::Container>(sp);

                                if (isMouseOver(c.get()->get("search_button")) &&
                                     m_menu->getMenuItem(Menu::Item::SearchPanel)->isVisible() &&
                                      getState() == State::Chatting)
                                    return UserInterface::UIEvent::Search;

                                if (isMouseOver(c.get()->get("add_friend_button")) &&
                                     m_menu->getMenuItem(Menu::Item::SearchPanel)->isVisible() &&
                                      getState() == State::Chatting)
                                    return UserInterface::UIEvent::AddFriend;

                                // get current mouse poitner position in the render window
                                sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);

                                // if the mouse poitner is over friendlist, and
                                // friendlist is visible, and the selected item
                                // in friendlist is not a whitespace, then add
                                // a corresppnding tab for the selected friend.

                                // NOTE : There's a bug in this if check
                                if (m_menu->getFriendlist()->getBounds().contains(mousePos.x, mousePos.y) && m_menu->getFriendlist()->isVisible() && m_menu->getSelectedFriend() != "")
                                {
                                    if (m_tabs->isTabPresent(m_menu->getSelectedFriend()))
                                    {
                                        m_tabs->focusTab(m_menu->getSelectedFriend());
                                        m_menu->getFriendlist()->hide();
                                        return UIEvent::TabSelected;
                                    }
                                    else
                                    {
                                        m_tabs->addTab(m_menu->getSelectedFriend(), m_menu->getStatus(m_menu->getSelectedFriend()));
                                        m_menu->getFriendlist()->hide();
                                        return UIEvent::TabSelected;
                                    }

                                    m_menu->getFriendlist()->hide();
                                }

                                else if (m_tabs->mouseOnWidget(mousePos.x, mousePos.y) && m_tabs->getTabCount() > 0)
                                    return UIEvent::TabSelected;

                                if (m_menu->getFriendlist()->isVisible() && !(m_menu->getFriendlist()->getBounds().contains(mousePos.x, mousePos.y) || m_menu->getItemBounds(0).contains(mousePos.x, mousePos.y)))
                                    m_menu->getFriendlist()->hide();

                                auto bounds = sf::FloatRect{m_menu->getMenuItem(Menu::Item::SearchPanel)->getPosition().x,
                                                            m_menu->getMenuItem(Menu::Item::SearchPanel)->getPosition().y,
                                                            m_menu->getMenuItem(Menu::Item::SearchPanel)->getSize().x,
                                                            m_menu->getMenuItem(Menu::Item::SearchPanel)->getSize().y,
                                                           };
                                if (m_menu->getMenuItem(Menu::Item::SearchPanel)->isVisible() && !(bounds.contains(mousePos.x, mousePos.y) || m_menu->getItemBounds(1).contains(mousePos.x, mousePos.y)))
                                    m_menu->getMenuItem(Menu::Item::SearchPanel)->hide();
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            // If the user is logged in and chatting, and if
            // no tabs are open, no need to have the chatbox
            // and input box open. So hide them.
            if (m_state == State::Chatting)
            {
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
                    //m_chatBox->setText("");

                    m_chatMessage->show();
                    m_chatBox->hide();
                    m_inputBox->hide();
                    m_chatWindowBorder->hide();
                }
            }
            else
            {

            }

            // If no UI event occurred, return a None event
            return UIEvent::None;
        }

        // if renderwindow was closed, then return the Closed UI event
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
        m_alertMessage->setText(message);
        m_alertBox->setSize(m_alertMessage->getSize().x + 100, m_alertMessage->getSize().y + 100);
        m_alertMessage->setPosition(50, 50);
        m_alertBox->setPosition(tgui::bindWidth(m_gui) / 2 - m_alertBox->getSize().x / 2,
                                tgui::bindHeight(m_gui) / 2 - m_alertBox->getSize().y / 2);
        m_alertBox->show();
        m_alertBox->moveToFront();

        for (auto w : m_gui.getWidgets())
            w->disable();

        m_alertBox->enable();
    }

    void GraphicalUI::setInputText(const std::string& str)
    {
        m_inputBox->setText(str);
    }

    std::string GraphicalUI::getInputText()
    {
        return m_inputBox->getText();
    }

    std::string GraphicalUI::getUsername()
    {
        return m_usernameField->getText();
    }

    std::string GraphicalUI::getSelectedFriend()
    {
        return m_tabs->getSelectedTabLabel();
    }

    std::string GraphicalUI::getPassword()
    {
        return m_passwordField->getText();
    }

    std::string GraphicalUI::getSearchString()
    {
        return m_menu->getSearchFieldText();
    }

    void GraphicalUI::clearChat()
    {
        m_chatBox->setText("");
    }

    std::string GraphicalUI::getChat(const std::string& user)
    {
        return m_chatBox->getText();
    }

    void GraphicalUI::addToChatArea(const std::string& text)
    {
        if (isStringWhitespace(m_chatBox->getText()))
            m_chatBox->addText(text);
        else
            m_chatBox->addText("\n" + text);
    }

    bool GraphicalUI::isStringWhitespace(const std::string& str)
    {
        for (auto& i : str)
        {
            if (i != ' ' && i != '\t' && i != '\n')
            {
                return false;
            }
        }
        return true;
    }

    void GraphicalUI::fillFriendList(const std::vector<std::string>& friends)
    {
        m_menu->initList(friends);
    }

    void GraphicalUI::closeAlert()
    {
        m_alertBox->hide();

        for (auto w : m_gui.getWidgets())
            w->enable();
    }

    UserInterface::UIEvent GraphicalUI::getUIEvent(const std::string& button)
    {
        std::cout << "A" << std::endl;
        if (button == "login")
            return UserInterface::UIEvent::UserLogin;
        else if (button == "signup_screen")
            return UserInterface::UIEvent::StateChanged;
        else if (button == "signup")
            return UserInterface::UIEvent::UserSignup;
        else if (button == "back")
            return UserInterface::UIEvent::UserLogin;
        else if (button == "logout")
            return UserInterface::UIEvent::Disconnect;
    }

    void GraphicalUI::setStatusOfFriend(const std::string& sender, unsigned int status)
    {
        m_menu->getFriendlist()->setStatusOfItem(sender, status);
        m_tabs->setStatusOfItem(sender, status);
    }

    void GraphicalUI::showSearchResults(const std::vector<std::string>& results)
    {
        m_menu->showSearchResults(results);
    }

    std::string GraphicalUI::getSelectedResult()
    {
        return m_menu->getSelectedResult();
    }

    void GraphicalUI::addFriend(const std::string& friendName)
    {
        std::static_pointer_cast<GraphicList>(m_menu->getMenuItem(Menu::Item::FriendPanel))->addItem(friendName);
    }
}
