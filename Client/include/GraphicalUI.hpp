/**
*
*    GraphicalUI
*   =============
*
*   The UI class which is used when the Prattle Client uses a GUI.
*
*/

#ifndef GRAPHICALUI_HPP
#define GRAPHICALUI_HPP

#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "../include/UserInterface.hpp"
#include "../include/GraphicList.hpp"
#include "../include/GraphicTab.hpp"
#include "../include/Menu.hpp"

/*
    For un-defining None and Status which conflict with SFML's
    sf::Socket::Status and sf::Socket::Status::None.
*/
#if defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD)
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #undef None
    #undef Status
#endif

namespace prattle
{
    /*
        Class GraphicalUI. Has the sf::RenderWindow, the TGUI widgets
        (including some custom widgets) and a load of constants
        associated with the GUI.
    */

    class GraphicalUI : public UserInterface
    {
        public:

            /*
                Public GUI constants. Variables are self-descriptive.
            */

            const unsigned short MIN_WINDOW_WIDTH  = 800;
            const unsigned short MIN_WINDOW_HEIGHT = 600;
            const unsigned short MIN_WINDOW_BPP    = 32;
            const std::string    TITLE             = "Prattle - Always be Near";

            const std::string BACKGROUND         = "resources/images/prattle-bg.png";
            const std::string INACTIVE_FILTER    = "resources/images/prattle-inactive-layer.png";
            const std::string FRAME              = "resources/images/frame.png";
            const std::string LOGO               = "resources/images/prattle-logo.png";
            //const std::string DEFAULT_TGUI_THEME = "resources/widgets/Black.conf";
            const std::string DEFAULT_TGUI_THEME = "resources/widgets/Black.txt";
            const std::string ALERT_FRAME        = "resources/images/prattle-alert-box.png";

            const std::string DEFAULT_GLOBAL_FONT    = "resources/fonts/Roboto-Bold.ttf";
            const std::string CLOSE_BUTTON_GRAPHIC   = "resources/images/custom/close_button.png";
            const std::string GRAPHIC_LISTITEM       = "resources/images/custom/graphic_list_item.png";
            const std::string GRAPHIC_LISTITEM_PANEL = "resources/images/custom/graphic_list_panel.png";
            const std::string GRAPHIC_TAB            = "resources/images/custom/graphic_tab.png";
            const std::string GRAPHIC_TAB_ACTIVE     = "resources/images/custom/graphic_tab_active.png";
            const std::string MENU_ITEMS             = "resources/images/custom/menu_items.png";
            const std::string SCROLLER               = "resources/images/custom/scroller.png";
            const std::string ARROWS                 = "resources/images/custom/tab_arrow.png";
            const std::string CHAT_WINDOW_BORDER     = "resources/images/custom/chat_window_border.png";

        public:
            // member methods
                 GraphicalUI();
            void reset() override;
            bool isMouseOver(tgui::Widget::Ptr widget);

            // virtually inherited functions
            void setState(UserInterface::State s) override;
            UserInterface::State getState() override;
            UserInterface::UIEvent update() override;
            void draw() override;

            void alert(const std::string& message, bool choice) override;
            bool isAutoLoginEnabled() override;
            void enableAutoLogin(bool enable) override;

            void setInputBufferText(const std::string& user, const std::string& message) override;
            std::string getInputBufferText(const std::string& user) override;
            void setInputText(const std::string& str) override;
            std::string getInputText() override;
            std::string getUsername() override;
            void setUsernameLabel(const std::string& username) override;
            std::string getSelectedFriend() override;
            std::string getPassword() override;
            std::string getSearchString() override;

            void clearChat() override;
            std::string getChat(const std::string& user) override;
            void addToChatArea(const std::string& text) override;

            bool isStringWhitespace(const std::string& str) override;

            void fillFriendList(const std::vector<std::string>& friends) override;

            void setStatusOfFriend(const std::string& sender, unsigned int status) override;

            void showSearchResults(const std::vector<std::string>& results) override;

            std::string getSelectedResult() override;

            std::string getSelectedFriendItem() override;

            void addFriend(const std::string& friendName) override;

            void insertNotif(const std::string& friendName, const std::string& notif = "") override;

            void setFriendActive(const std::string& friendName, bool active = false) override;

            void setSpaceInactive(bool active);

            //std::string getFocusedTab() override;

            // GUI only methods
            void animateButton(tgui::Button::Ptr button);
            void animatePanel(tgui::Panel::Ptr panel);

            // temp functions
            void deselectAll() override;

        private:
            void closeAlert();

            // NOTE : This function is useless right now
            //UserInterface::UIEvent getUIEvent(const std::string& button);

        private:

            UserInterface::State m_state;

            // The render window
            sf::RenderWindow m_window;

            // TGUI members. These are normal widgets (buttons, editboxes etc)

            tgui::Gui m_gui;

            tgui::Theme::Ptr m_theme;

            // The different screenstates
            tgui::Panel::Ptr m_loginScreen;
            tgui::Panel::Ptr m_signupScreen;
            tgui::Panel::Ptr m_connectingScreen;
            tgui::Panel::Ptr m_chatScreen;

            // the widets
            tgui::Picture::Ptr     m_background;
            tgui::Picture::Ptr     m_inactiveFilter;
            tgui::Picture::Ptr     m_logo;
            tgui::Picture::Ptr     m_frame;
            tgui::Label::Ptr       m_loginMessage;
            tgui::EditBox::Ptr     m_usernameField;
            tgui::EditBox::Ptr     m_passwordField;
            tgui::Button::Ptr      m_loginButton;
            //tgui::Checkbox::Ptr    m_rememberMeCheckbox;
            tgui::CheckBox::Ptr    m_rememberMeCheckbox;
            tgui::Label::Ptr       m_signupMessage;
            tgui::Button::Ptr      m_signupScreenButton;    // take the user from the login screen to the signup screen
            tgui::Label::Ptr       m_signupDetailsLabel;    // submit the details to server
            tgui::Button::Ptr      m_signupButton;
            tgui::Button::Ptr      m_backButton;

            tgui::Label::Ptr       m_connectingText;

            Menu::Ptr              m_menu;
            tgui::Label::Ptr       m_connectedUser;
            tgui::Button::Ptr      m_logoutButton;
            GraphicTab::Ptr        m_tabs;
            tgui::TextBox::Ptr     m_chatBox;
            tgui::TextBox::Ptr     m_inputBox;
            std::map<std::string, std::string> m_inputBuffer;
            tgui::Picture::Ptr     m_chatWindowBorder;
            tgui::Label::Ptr       m_chatMessage;
            tgui::ChildWindow::Ptr m_alertBox;
            tgui::Picture::Ptr     m_alertFrame;
            tgui::Label::Ptr       m_alertMessage;
            tgui::Button::Ptr      m_alertYesButton;
            tgui::Button::Ptr      m_alertNoButton;
    };
}

#endif // GRAPHICALGUI_HPP
