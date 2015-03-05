/**

    Prattle/Client/UI.cpp
    =====================

    Contains implementations of class UI declared in Prattle/Client/UI.hpp

*/

#include "../include/UI.hpp"

namespace prattle
{
    UI::UI() : m_width{800}
             , m_height{600}
             , m_bpp{32}
             , m_title{"Prattle - Always be Near [version 0.1]"}
             , m_gui{}
    {
        m_window.create(sf::VideoMode(m_width, m_height, m_bpp), m_title, sf::Style::Default);

        /* Initializing the GUI */

        // Initialize the SFML window as the renderer for the TGUI widgets
        m_gui.setWindow(m_window);
        // Set the global font for TGUI
        m_gui.setGlobalFont(DEFAULT_GLOBAL_FONT);

        //m_background = tgui::Picture::create(DEFAULT_BACKGROUND);
        //m_gui.add(m_background);

        // Initialize the panels which serve as the login screen, the signup
        // screen and the chat screen. Add them to TGUI
        m_loginPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_loginPanel);//, "login_panel");

        m_registerPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_registerPanel);//, "register_panel");

        m_chatPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_chatPanel);//, "chat_panel");

        // The background image of the application
        m_background = tgui::Picture::create(DEFAULT_BACKGROUND);
        //m_gui.add(m_background);
        //m_background->moveToBack();

        // The logo of the application
        m_logo = tgui::Picture::create(DEFAULT_LOGO);
        //m_logo->scale(0.8, 0.8);
        m_logo->setSize(400, 250);
        m_logo->setSmooth(true);
        //m_logo->setPosition(tgui::bindRight(m_gui) - m_logo->getSize().x, tgui::bindHeight(m_gui) / 4 - 80);
        m_logo->getTooltip()->setText("Always be near");
        //m_gui.add(m_logo);

        // The motto of Prattle (Always be near)
        //m_caption = tgui::Label::create(DEFAULT_TGUI_THEME);
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
        //m_loginPanel->add(m_caption);
        m_loginPanel->add(m_loginMsg);
        m_loginPanel->add(m_usernameField);
        m_loginPanel->add(m_passwordField);
        m_loginPanel->add(m_loginButton);
        m_loginPanel->add(m_rememberMeCheckbox);
        m_loginPanel->add(m_registerMsg);
        m_loginPanel->add(m_signUpButton);

        // Widgets for signup screen
        m_signUpMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_submitButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_backButton = tgui::Button::create(DEFAULT_TGUI_THEME);

        // Add all these widgets to the signup panel so they get
        // displayed in the signup screen
        m_registerPanel->add(m_background);
        m_registerPanel->add(m_logo);
        //m_registerPanel->add(m_caption);
        m_registerPanel->add(m_signUpMsg);
        m_registerPanel->add(m_submitButton);
        m_registerPanel->add(m_backButton);

        // Chat screen related widgets
        m_userNameLabel = tgui::Label::create();
        m_logoutButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_friendlistPanel = tgui::Panel::create(sf::Vector2f(270, 300));
        m_friendListVisibilityButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_friendList = tgui::ListBox::create(DEFAULT_TGUI_THEME);
        m_friendChatTabs = tgui::Tab::create();
        m_infoLabel = tgui::Label::create();
        m_friendsPanelVisibility = true;

        m_searchBox = tgui::EditBox::create();//DEFAULT_TGUI_THEME);
        m_searchButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_searchResults = tgui::ListBox::create(DEFAULT_TGUI_THEME);
        m_searchMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_addFriendButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_searchPanel = tgui::Panel::create(sf::Vector2f(0, 0));
        m_searchWindowVisibilityButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_searchPanelVisibility = true;

        m_chatBox = tgui::TextBox::create();
        m_inputTextBox = tgui::TextBox::create();
        m_initialMsg = tgui::Label::create();

        // Add these widgets to the chat panel
        m_chatPanel->add(m_background);
        m_chatPanel->add(m_userNameLabel);
        m_chatPanel->add(m_logoutButton);
        m_chatPanel->add(m_friendlistPanel);
        m_chatPanel->add(m_friendChatTabs);
        m_chatPanel->add(m_friendListVisibilityButton);
        m_chatPanel->add(m_searchPanel);
        m_chatPanel->add(m_searchWindowVisibilityButton);
        m_chatPanel->add(m_chatBox);
        m_chatPanel->add(m_inputTextBox, "input_textbox");
        m_chatPanel->add(m_initialMsg);

        m_loginPanel->hide();
        m_registerPanel->hide();
        m_chatPanel->hide();

        // Create the message window for displaying pop-up messages to the user
        m_messageWindow = tgui::ChildWindow::create(DEFAULT_TGUI_THEME);
        m_gui.add(m_messageWindow);
        //m_messageWindow->setTextColor(sf::Color::White);
        m_messageWindow->hide();
        m_messageWindow->keepInParent(true);

        m_messageLabel = tgui::Label::create();
        m_messageLabel->setTextColor(sf::Color::White);
        m_messageWindow->add(m_messageLabel);

        m_signUpButton->connect("pressed", &UI::changeScreenState, this, ScreenState::SignupScreen);
        m_backButton->connect("pressed", &UI::changeScreenState, this, ScreenState::LoginScreen);
        m_messageWindow->connect("Closed", std::bind(&tgui::ChildWindow::hide, m_messageWindow));
        m_friendListVisibilityButton->connect("pressed", &UI::togglePanelVisibility, this, m_friendlistPanel);
        m_searchWindowVisibilityButton->connect("pressed", &UI::togglePanelVisibility, this, m_searchPanel);
        m_friendChatTabs->connect("TabChanged", &UI::reloadChat, this);

        // Change the screen state to the login screen
        m_screenState = ScreenState::LoginScreen;
        changeScreenState(m_screenState);
    }

    void UI::reset()
    {
        /** The following for setting minimum window size is applicable only for X window systems **/
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

        m_loginMsg->setPosition(tgui::bindWidth(m_gui) / 9, tgui::bindHeight(m_gui) / 2.25);
        m_loginMsg->setText("Login to start prattling");
        m_loginMsg->setTextSize(22);
        m_loginMsg->setTextColor(sf::Color::White);

        m_usernameField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 15);
        m_usernameField->setPosition(tgui::bindWidth(m_gui) / 10, tgui::bindHeight(m_gui) / 1.85);
        m_usernameField->setDefaultText("Username");
        m_usernameField->setText("");

        m_passwordField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_usernameField));
        m_passwordField->setPosition(tgui::bindWidth(m_gui) / 10, tgui::bindBottom(m_usernameField) + 10);
        m_passwordField->setDefaultText("Password");
        m_passwordField->setPasswordCharacter('*');
        m_passwordField->setText("");

        m_loginButton->setText("Login");
        m_loginButton->setSize(tgui::bindWidth(m_passwordField) - 100, tgui::bindHeight(m_passwordField) - 10);
        m_loginButton->setPosition(tgui::bindWidth(m_gui) / 6, tgui::bindBottom(m_passwordField) + 20);
        m_loginButton->getTooltip()->setText("Login to start prattling");

        m_rememberMeCheckbox->setText("Remember me");
        m_rememberMeCheckbox->setSize(tgui::bindWidth(m_passwordField) / 10 - 20, tgui::bindHeight(m_passwordField) - 20);
        m_rememberMeCheckbox->setPosition(tgui::bindLeft(m_passwordField) + 50, tgui::bindBottom(m_loginButton) + 10);
        //m_rememberMeCheckbox->allowTextClick(true);

        m_registerMsg->setPosition(tgui::bindWidth(m_gui) / 3.4, tgui::bindHeight(m_gui) / 1.09);
        m_registerMsg->setText(L"Need an account? Then signup!");
        m_registerMsg->setTextSize(15);
        m_registerMsg->setTextColor(sf::Color::White);

        m_signUpButton->setText("Sign Up");
        m_signUpButton->setTextSize(15);
        m_signUpButton->setSize(tgui::bindWidth(m_passwordField) / 2, tgui::bindHeight(m_passwordField) - 10);
        m_signUpButton->setPosition(tgui::bindRight(m_registerMsg) + 20 , tgui::bindHeight(m_gui) / 1.10 - 3);

        // Add all these widgets to the login panel so that they get
        // displayed in the login screen

        // Widgets for signup screen

        m_signUpMsg->setPosition(tgui::bindWidth(m_gui) / 9, tgui::bindHeight(m_gui) / 2.25);
        m_signUpMsg->setText("Fill your details");
        m_signUpMsg->setTextSize(22);
        m_signUpMsg->setTextColor(sf::Color::White);

        m_submitButton->setText("Sign Up");
        m_submitButton->setTextSize(15);
        m_submitButton->setSize(tgui::bindWidth(m_passwordField), tgui::bindHeight(m_passwordField));
        m_submitButton->setPosition(tgui::bindLeft(m_passwordField), tgui::bindHeight(m_gui) / 1.3 - 3);

        m_backButton->setText(L"\u21BA Back");
        m_backButton->setTextSize(15);
        m_backButton->setSize(100, 30);
        m_backButton->setPosition(tgui::bindLeft(m_submitButton), tgui::bindHeight(m_gui) / 1.15);

        // Chat screen related widgets

        m_userNameLabel->setText("Logged in as : ");
        m_userNameLabel->setTextSize(15);
        m_userNameLabel->setTextColor(sf::Color::White);
        m_userNameLabel->setPosition(m_window.getSize().x - 100 - (40 + 100 + 20), 40);

        m_logoutButton->setText("Logout");
        m_logoutButton->setTextSize(15);
        m_logoutButton->setSize(100, 25);
        m_logoutButton->setPosition(tgui::bindWidth(m_gui) - 150, m_userNameLabel->getPosition().y - 6);

        m_friendsPanelVisibility = true;

        m_friendList->removeAllItems();
        m_friendList->setSize(250, m_friendList->getItemCount() * 40);
        //if (m_friendList->getItemCount() != 0)
        m_friendList->removeAllItems();
        m_friendList->setPosition(10, 50);

        m_friendlistPanel->setSize(tgui::bindWidth(m_friendList) + 20, tgui::bindHeight(m_friendList) + 20);
        m_friendlistPanel->setPosition(tgui::bindLeft(m_friendListVisibilityButton),
                                        tgui::bindBottom(m_friendListVisibilityButton) + 5);
        m_friendlistPanel->add(m_friendList);
        m_friendlistPanel->add(m_infoLabel);
        m_friendlistPanel->moveToFront();
        m_friendlistPanel->hide();

        m_friendListVisibilityButton->setText("My Buddies");
        m_friendListVisibilityButton->setTextSize(12);
        m_friendListVisibilityButton->setSize(120, 30);
        m_friendListVisibilityButton->setPosition(40, tgui::bindHeight(m_logoutButton) + 20);

        m_noOfTabs = 0;

        m_friendChatTabs->removeAll();
        m_friendChatTabs->setSize(200, 50);
        m_friendChatTabs->setTextSize(12);
        m_friendChatTabs->setTabHeight(20);
        m_friendChatTabs->setPosition(tgui::bindLeft(m_friendListVisibilityButton),
                                        tgui::bindTop(m_friendListVisibilityButton) +
                                            m_friendListVisibilityButton->getSize().y + 18);
        m_friendChatTabs->hide();

        m_infoLabel->setText("All buddies:");
        m_infoLabel->setTextSize(14);
        m_infoLabel->setPosition(10, 20);

        m_searchPanelVisibility = true;

        m_searchPanel->setPosition(tgui::bindLeft(m_searchWindowVisibilityButton), tgui::bindBottom(m_searchWindowVisibilityButton) + 5);
        m_searchPanel->add(m_searchBox);
        m_searchPanel->add(m_searchButton);
        m_searchPanel->add(m_searchResults);
        m_searchPanel->add(m_addFriendButton);
        m_searchPanel->moveToFront();
        m_searchPanel->hide();

        m_searchWindowVisibilityButton->setText("Search Prattlers");
        m_searchWindowVisibilityButton->setTextSize(12);
        m_searchWindowVisibilityButton->setSize(120, 30);
        m_searchWindowVisibilityButton->setPosition(tgui::bindRight(m_friendListVisibilityButton) + 20, tgui::bindTop(m_friendListVisibilityButton));

        m_searchBox->setDefaultText("Search username");
        m_searchBox->setText("");
        m_searchBox->setTextSize(20);
        m_searchBox->setSize(250, 25);
        m_searchBox->setPosition(20, 30);

        m_searchButton->setText("Search");
        m_searchButton->setTextSize(14);
        m_searchButton->setSize(100, 30);
        m_searchButton->setPosition(tgui::bindRight(m_searchBox) - 100, tgui::bindBottom(m_searchBox) + 10);

        m_searchResults->removeAllItems();
        m_searchResults->setSize(260, 0);
        m_searchResults->setPosition(tgui::bindLeft(m_searchBox), tgui::bindBottom(m_searchButton) + 20);

        m_searchMsg->setTextSize(14);
        m_searchMsg->setPosition(tgui::bindLeft(m_searchBox), tgui::bindBottom(m_searchButton) + 20);

        m_addFriendButton->setText("Add as friend");
        m_addFriendButton->setTextSize(14);
        m_addFriendButton->setSize(200, 30);
        m_addFriendButton->setPosition(tgui::bindLeft(m_searchPanel) - 100, tgui::bindBottom(m_searchResults) + 10);

        m_searchPanel->setSize(300, m_searchBox->getSize().y + m_searchResults->getSize().y + m_searchButton->getSize().y + 20);
        m_searchPanel->setPosition(tgui::bindLeft(m_searchWindowVisibilityButton), tgui::bindBottom(m_searchWindowVisibilityButton) + 5);
        m_searchPanel->hide();

        //std::cout << m_searchPanel->getSize().y << std::endl;

        m_chatBox->setSize(tgui::bindWidth(m_gui) - 80, tgui::bindHeight(m_gui) / 1.6);
        m_chatBox->setTextSize(15);
        m_chatBox->setText("");
        m_chatBox->setPosition(40, tgui::bindHeight(m_gui) / 5.2);
        m_chatBox->setReadOnly(true);
        m_chatBox->hide();

        m_inputTextBox->setSize(tgui::bindWidth(m_gui) - 80, tgui::bindHeight(m_gui) / 8);
        m_inputTextBox->setTextSize(15);
        m_inputTextBox->setText("");
        m_inputTextBox->setPosition(40, tgui::bindBottom(m_chatBox) + 10);
        m_inputTextBox->hide();

        m_initialMsg->setText("Welcome Prattler! Please select a prattler to chat with from the\n  Buddy list or click the 'Gobal chat' button to join open chat.");
        m_initialMsg->setTextSize(15);
        m_initialMsg->setTextColor(sf::Color::White);
        m_initialMsg->setPosition(tgui::bindWidth(m_gui) / 2 - m_initialMsg->getSize().x / 2, tgui::bindHeight(m_gui) / 2 - 20);

        // Change the screen state to the login screen
        m_screenState = ScreenState::LoginScreen;
        //m_screenState = ScreenState::ChatScreen;
        changeScreenState(m_screenState);
    }

    tgui::ListBox::Ptr UI::getFriendListPtr()
    {
        return m_friendList;
    }

    tgui::Tab::Ptr UI::getFriendTabPtr()
    {
        return m_friendChatTabs;
    }

    void UI::reloadChat()
    {
        clearChatBox();
        addTextToChatBox(m_chatHistory.find(m_friendChatTabs->getSelected())->second);
    }

    void UI::updateWidgets()
    {
        if (m_friendList->getItemCount() == 0)
        {
            m_friendList->hide();
            m_infoLabel->setText("You don't have any buddies (yet)! :)");
            m_initialMsg->show();
        }

        else
        {
            m_friendChatTabs->show();
            m_friendList->show();
            m_infoLabel->setText("All buddies :");
        }

        if (m_friendList->getSelectedItem() != "")
        {
            insertNewFriendTab(m_friendList->getSelectedItem());
            m_friendList->deselectItem();
        }

        if (m_friendChatTabs->getSelected() == "")
        {
            m_initialMsg->show();
            m_friendChatTabs->hide();
        }

        else
        {
            m_friendChatTabs->show();
            m_initialMsg->hide();
            m_chatBox->show();
            m_inputTextBox->show();

        }

        if (m_searchResults->getSelectedItem() != "")
            m_addFriendButton->show();
        else
            m_addFriendButton->hide();

        // Beware of test code for GUI
        m_friendList->setSize(250, m_friendList->getItemCount() * 30);
        if (m_friendList->getFullSize().y + 100 < m_width - 200)
            m_friendlistPanel->setSize(m_friendlistPanel->getSize().x, m_friendList->getFullSize().y + 100);
        else
            m_friendlistPanel->setSize(m_friendlistPanel->getSize().x, m_width - 400);

        m_searchPanel->setSize(300, m_searchBox->getSize().y + m_searchResults->getFullSize().y + m_searchButton->getSize().y + m_addFriendButton->getSize().y + 100);

        m_loginPanel->setSize(tgui::bindWidth(m_gui), tgui::bindHeight(m_gui));
        m_registerPanel->setSize(tgui::bindWidth(m_gui), tgui::bindHeight(m_gui));
        m_chatPanel->setSize(tgui::bindWidth(m_gui), tgui::bindHeight(m_gui));

        if (m_window.getSize().x >= 1000)
            m_logo->setSize(640, 400);
        else
            m_logo->setSize(400, 250);

        m_logo->setPosition(tgui::bindRight(m_gui) - m_logo->getSize().x, tgui::bindHeight(m_gui) / 4);

        m_loginButton->setPosition(tgui::bindWidth(m_gui) / 7.3, tgui::bindBottom(m_passwordField) + 20);
        m_loginButton->setSize(tgui::bindWidth(m_passwordField) / 1.3, tgui::bindHeight(m_passwordField) - 10);

        m_userNameLabel->setPosition(tgui::bindRight(m_gui) - m_userNameLabel->getSize().x - m_logoutButton->getSize().x - 60, m_userNameLabel->getPosition().y);

        //m_chatBox->setSize();
        /*if (now != "")
        {
            //if ()
        }*/
    }

    void UI::setChatUsername(const std::string& username)
    {
        m_userNameLabel->setText(m_userNameLabel->getText() + username);
        m_userNameLabel->setPosition(m_window.getSize().x - 100 - (40 + 100 + 20) - username.length() * 10, 40);
    }

    const std::string UI::getSelectedFriendTab()
    {
        return m_friendChatTabs->getSelected();
    }

    /*const std::string& UI::getSelectedFriend() const
    {
        return m_friendChatTabs->getSelected();
    }*/

    /*void UI::insertNotification(const std::string& username)
    {
        m_friendChatTabs->select(0);
        m_friendChatTabs->remove(username);
        m_friendChatTabs->add("(*)" + username);
        m_friendChatTabs->select(username);
    }*/

    void UI::insertNewFriendTab(const std::string& friendName)
    {
        if (m_friendChatTabs->getSelected() != "")
        {
            for (int i = 0; i <= m_noOfTabs; i++)
            {
                m_friendChatTabs->select(i);

                if (m_friendChatTabs->getSelected() == friendName)
                    return;
            }
        }

        m_friendChatTabs->add(friendName);
        m_noOfTabs++;
    }

    void UI::insertNewFriend(const std::string& friendName)
    {
        m_friendList->addItem(friendName);
    }

    void UI::addTextToChatBox(const std::string& message)
    {
        m_chatBox->addText(message);// + "\n");
    }

    /*void UI::addTextToChatBox(const std::string& user, const std::string& message)
    {
        m_chatBox->addText(user + " : " + message);// + "\n");
    }*/

    void UI::clearChatBox()
    {
        m_chatBox->setText("");
    }

    void UI::initFriendList(const std::vector<std::string>& friends)
    {
        for(auto& itr : friends)
        {
            m_friendList->addItem(itr);
        }

        m_friendList->show();
    }

    void UI::clearInputTextBox()
    {
        m_inputTextBox->setText("");
    }

    std::string UI::getSearchBoxText()
    {
        return m_searchBox->getText();
    }

    /*void UI::insertNewLine()
    {
        m_inputTextBox->addText("\n");
    }*/

    std::string UI::getInputText()
    {
        return m_inputTextBox->getText();
    }

    std::string UI::getUsernameFieldText()
    {
        return m_usernameField->getText();
    }

    std::string UI::getPasswordFieldText()
    {
        return m_passwordField->getText();
    }

    void UI::selectScreenState()
    {
        switch (m_screenState)
        {
            case ScreenState::LoginScreen:
                {
                    m_usernameField->getParent()->remove(m_usernameField);
                    m_passwordField->getParent()->remove(m_passwordField);

                    if (m_background->getParent() != nullptr)
                        m_background->getParent()->remove(m_passwordField);
                    if (m_logo->getParent() != nullptr)
                        m_logo->getParent()->remove(m_passwordField);

                    m_loginPanel->add(m_usernameField);
                    m_loginPanel->add(m_passwordField);

                    m_usernameField->setText("");
                    m_passwordField->setText("");

                    m_loginPanel->show();
                    m_registerPanel->hide();
                    m_chatPanel->hide();
                } break;

            case ScreenState::SignupScreen:
                {
                    m_usernameField->getParent()->remove(m_usernameField);
                    m_passwordField->getParent()->remove(m_passwordField);

                    m_registerPanel->add(m_usernameField);
                    m_registerPanel->add(m_passwordField);

                    m_usernameField->setText("");
                    m_passwordField->setText("");

                    m_loginPanel->hide();
                    m_registerPanel->show();
                    m_chatPanel->hide();
                } break;

            case ScreenState::ChatScreen:
                {
                    m_loginPanel->hide();
                    m_registerPanel->hide();
                    m_chatPanel->show();
                } break;
        }
    }

    void UI::changeScreenState(const ScreenState& screenState)
    {
        m_screenState = screenState;
        selectScreenState();
    }

    //void UI::togglePanelVisibility(tgui::Panel::Ptr panel, bool &visibility)
    void UI::togglePanelVisibility(tgui::Panel::Ptr panel)
    {
        /*if (visibility)
        {
            panel->show();
            visibility = false;
        }

        else
        {
            panel->hide();
            visibility= true;
        }*/
        if (m_friendsPanelVisibility)
        {
            panel->show();
            m_friendsPanelVisibility = false;
        }
        else
        {
            panel->hide();
            m_friendsPanelVisibility = true;
        }

        if (m_searchPanelVisibility)
        {
            panel->show();
            m_searchPanelVisibility = false;
        }
        else
        {
            panel->hide();
            m_searchPanelVisibility = true;
        }
    }

    tgui::Picture::Ptr UI::getBackground()
    {
        return m_background;
    }

    tgui::Gui* UI::getGui()
    {
        return &m_gui;
    }

    sf::RenderWindow* UI::getRenderWindow()
    {
        return &m_window;
    }
}
