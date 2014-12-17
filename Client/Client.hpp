/*

The client side application for chatting.

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include <TGUI/TGUI.hpp>
#include "System.hpp"

#if defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD)
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #undef None
    #undef Status
#endif

namespace prattle
{
    class Client
    {
        public:
            Client();

            bool checkIfWhitespace(const std::string& message);

            void run();

            void selectScreenForRendering();

            void changePanelVisibility(bool &visibility, tgui::Panel::Ptr panel);

            void changeScreenState(const ScreenState& screenState);

            void loginPrompt();

            void signup();

            bool login();

            bool isLoggedIn();

            sf::Socket::Status send(sf::Packet& packet);

            bool  receive();

            const std::string& getUserName();

            const std::string& getFriendName();

            bool  logout();

            void init();

        protected:

        private:

            /* Core members */
            sf::TcpSocket m_client;
            bool m_loginStatus;
            std::string m_userName;
            std::string m_password;
            std::vector<std::string> m_friends;
            Status m_onlineStatus;
            ScreenState m_screenState;

            /* GUI related members*/

            // Window related SFML objects

            sf::RenderWindow m_window;
            int m_width;
            int m_height;
            int m_bpp;
            std::string m_title;

            // Widgets related members

            tgui::Gui m_gui;
            tgui::Picture::Ptr m_background;
            tgui::Picture::Ptr m_logo;
            tgui::Label::Ptr m_caption;

            // Screen panels

            tgui::Panel::Ptr m_loginPanel;
            tgui::Panel::Ptr m_registerPanel;
            tgui::Panel::Ptr m_chatPanel;

            // Login screen related widgets

            tgui::Label::Ptr m_loginMsg;
            tgui::EditBox::Ptr m_usernameField;
            tgui::EditBox::Ptr m_passwordField;
            tgui::Button::Ptr m_loginButton;
            tgui::Checkbox::Ptr m_rememberMeCheckbox;
            tgui::Label::Ptr m_registerMsg;
            tgui::Button::Ptr m_signUpButton;

            // Registration screen related widgets

            tgui::Label::Ptr m_signUpMsg;
            tgui::EditBox::Ptr m_fullNameField;
            tgui::EditBox::Ptr m_newUsernameField;
            tgui::EditBox::Ptr m_newPasswordField;
            tgui::Button::Ptr m_submitButton;
            tgui::Button::Ptr m_backButton;

            // Chat screen related widgets

            tgui::Label::Ptr m_userNameLabel;
            tgui::Button::Ptr m_logoutButton;
            tgui::Tab::Ptr m_friendChatTabs;
            tgui::Button::Ptr m_friendListVisibilityButton;
            tgui::Panel::Ptr m_friendlistPanel;
            bool panelVisibility;
            tgui::ListBox::Ptr m_friendsOnline;
            tgui::TextBox::Ptr m_chatBox;
            tgui::TextBox::Ptr m_inputTextBox;

            // Message box
            tgui::ChildWindow::Ptr m_messageWindow;
            tgui::Label::Ptr m_messageLabel;
            tgui::Picture::Ptr m_messageClipArt;

            //for test purposes
            const std::string frnd1 = "raptor";
            const std::string frnd2 = "trex";
            const std::string frnd3 = "firefly";
    };
}

#endif // CLIENT_HPP
