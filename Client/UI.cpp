#include "UI.hpp"

namespace prattle
{
    UI::UI() : m_width{800},
               m_height{600},
               m_bpp{32},
               m_title{"Prattle - Always be Near [version 0.1]"},
               m_gui{}
    {
        m_window.create(sf::VideoMode(m_width, m_height, m_bpp), m_title);//, sf::Style::Close);

        /* Initializing the GUI */

        // Initialize the SFML window as the renderer for the TGUI widgets
        m_gui.setWindow(m_window);
        // Set the global font for TGUI
        m_gui.setGlobalFont(DEFAULT_GLOBAL_FONT);

        // Initialize the panels which serve as the login screen, the signup
        // screen and the chat screen. Add them to TGUI
        m_loginPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_loginPanel, "login_panel");

        m_registerPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_registerPanel, "register_panel");

        m_chatPanel = tgui::Panel::create(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_gui.add(m_chatPanel, "chat_panel");

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
        m_loginPanel->add(m_background, "background_tgui_picture");
        m_loginPanel->add(m_logo);
        m_loginPanel->add(m_caption);
        m_loginPanel->add(m_loginMsg);
        m_loginPanel->add(m_usernameField, "username_field");
        m_loginPanel->add(m_passwordField, "password_field");
        m_loginPanel->add(m_loginButton);
        m_loginPanel->add(m_rememberMeCheckbox);
        m_loginPanel->add(m_registerMsg);
        m_loginPanel->add(m_signUpButton);

        // Widgets for signup screen
        m_signUpMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_fullNameField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        //m_newUsernameField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        //m_newPasswordField = tgui::EditBox::create(DEFAULT_TGUI_THEME);
        m_submitButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_backButton = tgui::Button::create(DEFAULT_TGUI_THEME);

        // Add all these widgets to the signup panel so they get
        // displayed in the signup screen
        m_registerPanel->add(m_background);
        m_registerPanel->add(m_logo);
        m_registerPanel->add(m_caption);
        m_registerPanel->add(m_signUpMsg);
        m_registerPanel->add(m_fullNameField);
        //m_registerPanel->add(m_newUsernameField);
        //m_registerPanel->add(m_newPasswordField);
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
        friendsPanelVisibility = true;

        m_searchBox = tgui::EditBox::create();//DEFAULT_TGUI_THEME);
        m_searchButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        m_searchResults = tgui::ListBox::create(DEFAULT_TGUI_THEME);
        m_searchMsg = tgui::Label::create(DEFAULT_TGUI_THEME);
        m_searchPanel = tgui::Panel::create(sf::Vector2f(0, 0));
        m_searchWindowVisibilityButton = tgui::Button::create(DEFAULT_TGUI_THEME);
        searchPanelVisibility = true;

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

        //m_signUpButton->connect("pressed", &UI::signup this, std::string, std::string);
        //m_backButton->connect("pressed", &UI::changeScreenState, this, ScreenState::LoginScreen);
        //m_logoutButton->connect("pressed", &UI::logout, this);

        //m_loginButton->connect("pressed", std::bind(&Client::login, this, std::string, std::string);
        //m_signUpButton->connect("pressed", &sf::TcpSocket::send, packet);
        //m_signUpButton->connect("pressed", &Client::signup, this, std::string, std::string);
        //m_backButton->connect("pressed", &Client::changeScreenState, this, ScreenState::LoginScreen);
        //m_logoutButton->connect("pressed", &Client::logout, this);

        m_signUpButton->connect("pressed", &UI::changeScreenState, this, ScreenState::SignupScreen);
        m_backButton->connect("pressed", &UI::changeScreenState, this, ScreenState::LoginScreen);

        // Create the message window for displaying pop-up messages to the user
        m_messageWindow = tgui::ChildWindow::create();
        m_gui.add(m_messageWindow, "message_window");
        m_messageWindow->hide();
        m_messageWindow->keepInParent(true);

        m_messageLabel = tgui::Label::create();
        m_messageWindow->add(m_messageLabel);

        m_messageWindow->connect("Closed", std::bind(&tgui::ChildWindow::hide, m_messageWindow));

        // Change the screen state to the login screen
        m_screenState = ScreenState::LoginScreen;
        changeScreenState(m_screenState);
    }

    void UI::reset()
    {
        /** The following for setting minimum window size is applicable only for X based window systems **/
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
        #endif // defined

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
        m_usernameField->setText("");

        m_passwordField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_usernameField));
        m_passwordField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindBottom(m_usernameField) + 10);
        m_passwordField->setDefaultText("Password");
        m_passwordField->setPasswordCharacter('*');
        m_passwordField->setText("");

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

        //m_newUsernameField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 15);
        //m_newUsernameField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_gui) / 1.85);
        //m_newUsernameField->setDefaultText("Pick a username");

        //m_newPasswordField->setSize(tgui::bindWidth(m_gui) / 3, tgui::bindHeight(m_usernameField));
        //m_newPasswordField->setPosition(tgui::bindWidth(m_gui) / 3, tgui::bindBottom(m_usernameField) + 10);
        //m_newPasswordField->setDefaultText("Choose a password");
        //m_newPasswordField->setPasswordCharacter('*');

        m_submitButton->setText("Sign Up");
        m_submitButton->setTextSize(15);
        m_submitButton->setSize(tgui::bindWidth(m_passwordField), tgui::bindHeight(m_passwordField) + 10);
        m_submitButton->setPosition(tgui::bindWidth(m_passwordField), tgui::bindHeight(m_gui) / 1.3 - 3);

        m_backButton->setText("Back");
        m_backButton->setTextSize(15);
        m_backButton->setSize(100, 30);
        m_backButton->setPosition(tgui::bindWidth(m_passwordField) + 80, tgui::bindHeight(m_gui) / 1.15);

        m_userNameLabel->setText("Logged in as : ");// + m_userName);
        m_userNameLabel->setTextSize(15);
        m_userNameLabel->setTextColor(sf::Color::White);
        //m_userNameLabel->setPosition(m_window.getSize().x - 100 - (40 + 100 + 20) - m_userName.length() * 10, 40);
        m_userNameLabel->setPosition(m_window.getSize().x - 100 - (40 + 100 + 20), 40);

        m_logoutButton->setText("Logout");
        m_logoutButton->setTextSize(15);
        m_logoutButton->setSize(100, 25);
        m_logoutButton->setPosition(tgui::bindWidth(m_gui) - 150, m_userNameLabel->getPosition().y - 6);

        friendsPanelVisibility = true;

        //m_friendlistPanel->setSize(tgui::bindWidth(m_friendList) + 20, tgui::bindHeight(m_friendList) + 20);
        m_friendlistPanel->setPosition(tgui::bindLeft(m_friendListVisibilityButton),
                                        tgui::bindBottom(m_friendListVisibilityButton) + 5);
        m_friendlistPanel->add(m_friendList);
        m_friendlistPanel->add(m_infoLabel);
        m_friendlistPanel->hide();

        m_friendListVisibilityButton->setText("My Buddies");
        m_friendListVisibilityButton->setTextSize(12);
        m_friendListVisibilityButton->setSize(120, 30);
        m_friendListVisibilityButton->setPosition(40, tgui::bindHeight(m_logoutButton) + 20);
        m_friendListVisibilityButton->connect("pressed", &UI::changePanelVisibility, this, m_friendlistPanel, friendsPanelVisibility);

        m_friendList->setSize(250, 80);
        m_friendList->removeAllItems();
        m_friendList->setPosition(10, 50);

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

        searchPanelVisibility = true;

        //m_searchPanel->setSize(300, m_searchBox->getSize().y + m_searchResults->getSize().y + m_searchButton->getSize().y + 20);
        /*m_searchPanel->setPosition(tgui::bindLeft(m_searchWindowVisibilityButton), tgui::bindBottom(m_searchWindowVisibilityButton) + 5);
        m_searchPanel->add(m_searchBox);
        m_searchPanel->add(m_searchButton);
        m_searchPanel->add(m_searchResults);
        m_searchPanel->hide();*/

        m_searchWindowVisibilityButton->setText("Search Prattlers");
        m_searchWindowVisibilityButton->setTextSize(12);
        m_searchWindowVisibilityButton->setSize(120, 30);
        m_searchWindowVisibilityButton->setPosition(tgui::bindRight(m_friendListVisibilityButton) + 20, tgui::bindTop(m_friendListVisibilityButton));
        m_searchWindowVisibilityButton->connect("pressed", &UI::changePanelVisibility, this, m_searchPanel, searchPanelVisibility);

        m_searchBox->setDefaultText("Search username");
        m_searchBox->setTextSize(20);
        m_searchBox->setSize(250, 25);
        m_searchBox->setPosition(20, 30);

        m_searchButton->setText("Search");
        m_searchButton->setTextSize(14);
        m_searchButton->setSize(100, 30);
        m_searchButton->setPosition(tgui::bindRight(m_searchBox) - 100, tgui::bindBottom(m_searchBox) + 10);

        m_searchResults->setSize(260, 0);
        m_searchResults->setPosition(tgui::bindLeft(m_searchBox), tgui::bindBottom(m_searchButton) + 20);
        //std::cout << m_searchBox->getSize().y << " " << m_searchResults->getSize().y << " " << m_searchButton->getSize().y << std::endl;

        m_searchMsg->setTextSize(14);
        m_searchMsg->setPosition(tgui::bindLeft(m_searchBox), tgui::bindBottom(m_searchButton) + 20);

        m_searchPanel->setSize(300, m_searchBox->getSize().y + m_searchResults->getSize().y + m_searchButton->getSize().y + 100);
        m_searchPanel->setPosition(tgui::bindLeft(m_searchWindowVisibilityButton), tgui::bindBottom(m_searchWindowVisibilityButton) + 5);
        m_searchPanel->add(m_searchBox);
        m_searchPanel->add(m_searchButton);
        m_searchPanel->add(m_searchResults);
        m_searchPanel->add(m_searchMsg);
        m_searchPanel->hide();

        //std::cout << m_searchPanel->getSize().y << std::endl;

        m_chatBox->setSize(tgui::bindWidth(m_window) - 80, 320);
        m_chatBox->setTextSize(15);
        m_chatBox->setText("");
        m_chatBox->setPosition(40, tgui::bindHeight(m_gui) / 5.2);
        m_chatBox->setReadOnly(true);
        m_chatBox->hide();

        m_inputTextBox->setSize(tgui::bindWidth(m_window) - 80, 110);
        m_inputTextBox->setTextSize(15);
        m_inputTextBox->setText("");
        m_inputTextBox->setPosition(40, tgui::bindHeight(m_window) / 1.35);
        m_inputTextBox->hide();

        m_initialMsg->setText("Welcome Prattler! Please select a prattler to chat with from the\n  Buddy list or click the 'Gobal chat' button to join open chat.");
        m_initialMsg->setTextSize(15);
        m_initialMsg->setTextColor(sf::Color::White);
        m_initialMsg->setPosition(tgui::bindWidth(m_gui) / 2 - m_initialMsg->getSize().x / 2, tgui::bindHeight(m_gui) / 2 - 20);

        m_screenState = ScreenState::LoginScreen;
        //m_screenState = ScreenState::ChatScreen;
        changeScreenState(m_screenState);
    }

    sf::RenderWindow* UI::getRenderWindow()
    {
        return &m_window;
    }

    tgui::Gui* UI::getGui()
    {
        return &m_gui;
    }

    void UI::changePanelVisibility(tgui::Panel::Ptr panel, bool& visibility)
    {
        if (visibility)
        {
            panel->show();
            visibility = false;
        }
        else
        {
            panel->hide();
            visibility= true;
        }
    }

    void UI::changeScreenState(const ScreenState& screenState)
    {
        m_screenState = screenState;
        selectScreenForRendering();
    }

    void UI::selectScreenForRendering()
    {
        switch (m_screenState)
        {
            case ScreenState::LoginScreen:
                {
                    m_gui.remove(m_usernameField);
                    m_gui.remove(m_passwordField);

                    m_loginPanel->add(m_usernameField);
                    m_loginPanel->add(m_passwordField);

                    //m_usernameField->setText("");

                    m_loginPanel->show();
                    m_registerPanel->hide();
                    m_chatPanel->hide();
                } break;

            case ScreenState::SignupScreen:
                {
                    m_gui.remove(m_usernameField);
                    m_gui.remove(m_passwordField);

                    m_registerPanel->add(m_usernameField);
                    m_registerPanel->add(m_passwordField);

                    //m_usernameField->setText("");

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

    std::string UI::getUsernameFieldText()
    {
        return m_usernameField->getText();
    }

    std::string UI::getPasswordFieldText()
    {
        return m_passwordField->getText();
    }

    std::string UI::getInputText()
    {
        return m_inputTextBox->getText();
    }

    std::string UI::getSearchBoxText()
    {
        return m_searchBox->getText();
    }

    void UI::addTextToChatBox(const std::string& user, const std::string& message)
    {
        m_chatBox->addText(user + " : " + message);
    }

    void UI::clearInputTextBox()
    {
        m_inputTextBox->setText("");
    }

    void UI::insertNewFriendTab(const std::string& friendName)
    {
        m_friendList->addItem(friendName);//, "frnd1");
        m_friendChatTabs->add(friendName);
        //m_friendChatTabs->select(0);
    }

    void UI::setChatUsername(const std::string& username)
    {
        m_userNameLabel->setText(m_userNameLabel->getText() + username);
        m_userNameLabel->setPosition(m_window.getSize().x - 100 - (40 + 100 + 20) - username.length() * 10, 40);
    }

    void UI::updateWidgets()
    {
        if (m_friendChatTabs->getSelected() == "")
        {
            m_friendChatTabs->hide();
            m_friendList->hide();
            m_infoLabel->setText("You don't have any buddies (yet)! :)");
            m_initialMsg->show();
        }

        else
        {
            m_friendChatTabs->show();
            m_initialMsg->hide();
            m_infoLabel->setText("All buddies :");
        }

        m_friendlistPanel->setSize(m_friendlistPanel->getSize().x, 50);
        m_searchPanel->setSize(300, m_searchBox->getSize().y + m_searchResults->getSize().y + m_searchButton->getSize().y + 100);
    }
}
