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
#include "../include/System.hpp"

namespace prattle
{
    // Class UI which handles the GUI
    // part of the Client application.

    class UI
    {
        public:

            UI();                                   // The ctor initializes ALL the SFML and TGUI.

            void reset();                           // Reset all UI elements to a default state

            tgui::ListBox::Ptr getFriendListPtr();

            tgui::Tab::Ptr getFriendTabPtr();

            void updateWidgets();                   // Update widgets like the chat box etc that can change frequently.

            void setChatUsername(const std::string& username);
                                                    // Set the logged in as user to 'username'

            void insertNewFriendTab(const std::string& friendName);
                                                    // Insert a new chat tab

            void insertNewFriend(const std::string& friendName);
                                                    // Insert a new friend name in the friendlist

            void addTextToChatBox(const std::string& user,
                                  const std::string& message);
                                                    // Add a block of chat to the chat box.

            void initFriendList(const std::vector<std::string>& friends);
                                                    // Initializes the friendlist with already existing friends.

            void clearInputTextBox();               // Clear the text of the input textbox

            std::string getSearchBoxText();         // Get the text of the search field editbox

            std::string getInputText();             // Get the text of the input textbox

            std::string getUsernameFieldText();     // Get the text of the username editbox

            std::string getPasswordFieldText();     // Get the text of the password editbox

            void selectScreenState();               // Identify the screen state which needs to be used.

            void changeScreenState(const ScreenState& screenState);
                                                    // Change the current screenstate

            void togglePanelVisibility(tgui::Panel::Ptr panel);
                                                    // Toggle the visibility of a tgui::Panel::Ptr

            tgui::Picture::Ptr getBackground();    // Returns the background image

            tgui::Gui* getGui();                    // Return a pointer to the tgui:Gui object used for the GUI

            sf::RenderWindow* getRenderWindow();    // Return a pointer to the sf::RenderWindow object used for drawing

            // Public widgets accessible from the Client class
            tgui::Button::Ptr      m_loginButton;
            tgui::Button::Ptr      m_submitButton;
            tgui::Button::Ptr      m_logoutButton;
            tgui::ChildWindow::Ptr m_messageWindow;
            tgui::Label::Ptr       m_messageLabel;
            tgui::ListBox::Ptr     m_searchResults;
            tgui::Button::Ptr      m_searchButton;
            tgui::Label::Ptr       m_searchMsg;
            tgui::Button::Ptr      m_addFriendButton;

        private:

            // All the declarations below are self-descriptive

            // Window related SFML objects
            sf::RenderWindow m_window;
            unsigned int     m_width;
            unsigned int     m_height;
            unsigned int     m_bpp;
            std::string      m_title;
            ScreenState m_screenState;


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
            unsigned short     m_noOfTabs;
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
