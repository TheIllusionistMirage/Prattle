#ifndef UI_HPP
#define UI_HPP

#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "Config.hpp"
#include "System.hpp"

namespace prattle
{
    class Client;

    class UI
    {
        public:
            UI();

            //void render();
            //void update();
            void insertNewFriendTab(const std::string& friendName);
            void addTextToChatBox(const std::string& user, const std::string& message);
            void clearInputTextBox();
            std::string getInputText();
            std::string getUsernameFieldText();
            std::string getPasswordFieldText();
            void selectScreenForRendering();
            void changeScreenState(const ScreenState& screenState);
            tgui::Gui* getGui();
            sf::RenderWindow* getRenderWindow();
            void reset();

            friend bool logout();
            friend bool login(const std::string& username, const std::string& password);
            friend void signup(const std::string& username, const std::string& password);

            tgui::Button::Ptr m_loginButton;
            //tgui::Button::Ptr m_signUpButton;
            tgui::Button::Ptr m_submitButton;
            //tgui::Button::Ptr m_backButton;

        protected:
            void changePanelVisibility(tgui::Panel::Ptr panel, bool visibility);

        private:
            // Window related SFML objects

            sf::RenderWindow m_window;
            int m_width;
            int m_height;
            int m_bpp;
            std::string m_title;
            ScreenState m_screenState;

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
            //tgui::Button::Ptr m_loginButton;
            tgui::Checkbox::Ptr m_rememberMeCheckbox;
            tgui::Label::Ptr m_registerMsg;
            tgui::Button::Ptr m_signUpButton;

            // Registration screen related widgets

            tgui::Label::Ptr m_signUpMsg;
            tgui::EditBox::Ptr m_fullNameField;
            tgui::EditBox::Ptr m_newUsernameField;
            tgui::EditBox::Ptr m_newPasswordField;
            //tgui::Button::Ptr m_submitButton;
            tgui::Button::Ptr m_backButton;

            // Chat screen related widgets

            tgui::Label::Ptr m_userNameLabel;
            tgui::Button::Ptr m_logoutButton;
            tgui::Tab::Ptr m_friendChatTabs;
            tgui::Button::Ptr m_friendListVisibilityButton;
            tgui::Panel::Ptr m_friendlistPanel;
            bool friendsPanelVisibility;
            tgui::ListBox::Ptr m_friendsOnline;
            tgui::TextBox::Ptr m_chatBox;
            tgui::TextBox::Ptr m_inputTextBox;

            // Message box
            tgui::ChildWindow::Ptr m_messageWindow;
            tgui::Label::Ptr m_messageLabel;
            tgui::Picture::Ptr m_messageClipArt;
    };
}

#endif // UI_HPP
