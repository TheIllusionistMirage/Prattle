/*

The client side application for chatting.

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "System.hpp"

namespace chat
{
    class Client
    {
        public:
            Client();

            void loginPrompt();

            void signup();

            bool login();

            bool isLoggedIn();

            sf::Socket::Status send(sf::Packet& packet);

            bool receive();

            const std::string& getUserName();

            const std::string& getFriendName();

            void logout();

            /* GUI related methods */

            void renderScreen();

            void run();

        protected:
            void changeScreenState(chat::ScreenState screenState);

        private:

            /* Communication related members */

            sf::TcpSocket m_client;
            bool m_loginStatus;
            std::string m_userName;
            std::string m_password;
            //std::vector<std::string> m_friendList;
            std::string m_friend;
            OnlineStatus m_onlineStatus;

            /* GUI related members*/

            // Window related SFML objects

            sf::RenderWindow m_window;
            int m_width;
            int m_height;
            int m_bpp;
            std::string m_title;
            sf::Color DEFAULT_BG_COLOR = sf::Color::Black;
            sf::Color DEFAULT_FG_COLOR = sf::Color::White;

            // Widgets related members

            ScreenState m_screenState;

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
            tgui::TextBox::Ptr m_chatTextbox;
            tgui::Button::Ptr m_sendMsgButton;
    };
}

#endif // CLIENT_HPP
