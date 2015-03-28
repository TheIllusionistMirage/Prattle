/**

    Prattle/Client/UI.hpp
    =====================

    Manages the nice and visually appeasing UI of Prattle,
    with the help of TGUI, which is built upon SFML.

*/


#ifndef UI_HPP
#define UI_HPP

#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "../include/Config.hpp"

namespace prattle
{
    const std::string DEFAULT_GLOBAL_FONT = "resources/fonts/DejaVuSans.ttf";
    const std::string DEFAULT_BACKGROUND  = "resources/images/Untitled.png";
    const std::string DEFAULT_LOGO        = "resources/images/Prattle_Madly_Orange_logo_1.png";
    const std::string DEFAULT_TGUI_THEME  = "resources/widgets/Black.conf";

    enum class ScreenState
    {
        LoginScreen ,
        SignupScreen,
        ChatScreen
    };

    // Class UI which handles the GUI
    // part of the Client application.

    class UI
    {
        public:
                                UI();
            void                resetGUI();
            void                update();

            void                alert(const std::string& message);

            bool                isAutoLoginChecked();
            void                checkAutoLogin();
            void                reloadChat();
            void                setChatUsername(const std::string& username);
            void                insertUnreadMessageNotif(const std::string& username);
            //void                insertNotification(const std::string& notif);
            tgui::ListBox::Ptr  getFriendListPtr();
            tgui::Tab::Ptr      getFriendTabPtr();
            void                insertNewFriendTab(const std::string& friendName);
            void                addNewFriend(const std::string& friendName);
            void                initFriendList(const std::vector<std::string>& friends);

            void                addTextToChatBox(const std::string& message);
            void                clearInputTextBox();
            void                clearChatBox();
            std::string         getSearchBoxText();
            std::string         getInputText();
            std::string         getUsernameFieldText();
            std::string         getPasswordFieldText();

            const ScreenState&  getScreenState();
            void                setScreenState(ScreenState state);
            void                togglePanelVisibility(tgui::Panel::Ptr panel,
                                                      bool& visibility);

            tgui::Gui*          getGui();
            sf::RenderWindow*   getRenderWindow();

        protected:

            tgui::Button::Ptr      m_loginButton;
            tgui::Button::Ptr      m_submitButton;
            tgui::Button::Ptr      m_logoutButton;
            tgui::ChildWindow::Ptr m_messageWindow;
            tgui::Label::Ptr       m_messageLabel;
            tgui::ListBox::Ptr     m_searchResults;
            tgui::Button::Ptr      m_searchButton;
            tgui::Label::Ptr       m_searchMsg;
            tgui::Button::Ptr      m_addFriendButton;

            std::map<std::string, std::string> m_chatHistory;

        private:

            // All the declarations below are self-descriptive

            // Window related SFML objects
            unsigned int     m_width;
            unsigned int     m_height;
            unsigned int     m_bpp;
            std::string      m_title;
            sf::RenderWindow m_window;
            ScreenState      m_screenState;


            // Widgets related members

            tgui::Gui          m_gui;
            tgui::Picture::Ptr m_background;
            tgui::Picture::Ptr m_logo;


            // Screen panels
            tgui::Panel::Ptr m_loginPanel;
            tgui::Panel::Ptr m_registerPanel;
            tgui::Panel::Ptr m_chatPanel;


            // Login screen related widgets
            tgui::Label::Ptr    m_loginMsg;
            tgui::EditBox::Ptr  m_usernameField;
            tgui::EditBox::Ptr  m_passwordField;
            tgui::Checkbox::Ptr m_rememberMeCheckbox;
            tgui::Label::Ptr    m_registerMsg;
            tgui::Button::Ptr   m_signUpButton;


            // Registration screen related widgets
            tgui::Label::Ptr   m_signUpMsg;
            tgui::Button::Ptr  m_backButton;


            // Chat screen related widgets
            tgui::Label::Ptr   m_userNameLabel;
            tgui::Tab::Ptr     m_friendChatTabs;
            tgui::Button::Ptr  m_friendListVisibilityButton;
            tgui::Panel::Ptr   m_friendlistPanel;
            tgui::ListBox::Ptr m_friendList;
            tgui::Label::Ptr   m_infoLabel;
            bool               m_friendsPanelVisibility;
            tgui::EditBox::Ptr m_searchBox;
            tgui::Panel::Ptr   m_searchPanel;
            tgui::Button::Ptr  m_searchWindowVisibilityButton;
            bool               m_searchPanelVisibility;
            tgui::TextBox::Ptr m_chatBox;
            tgui::TextBox::Ptr m_inputTextBox;
            tgui::Label::Ptr   m_initialMsg;
    };
}

#endif // UI_HPP
