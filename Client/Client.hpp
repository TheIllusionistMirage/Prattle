/**

    Prattle/Client/Client.hpp

    The client side application for chatting.

    Client contains two instances of Prattle/Client/NetworkManager and Prattle/Client/GUI each.
    NetworkManager handles the networking part while GUI is responsible for the graphics.

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "NetworkManager.hpp"
#include "UI.hpp"

namespace prattle
{
    class Client
    {
        public:
            Client();

            bool checkIfWhitespace(const std::string& message);
            bool isLoggedIn();
            std::string const& getUserName() const;
            std::string const& getFriendsName() const;
            bool logout();
            //bool login(const std::string& username, const std::string& password);
            bool login();
            //void signup(const std::string& username, const std::string& password);
            bool signup();

            void reset();
            void run();

        protected:

        private:
            bool m_loginStatus;
            std::string m_username;
            std::string m_password;
            std::string m_friend;
            std::vector<std::string> m_friends;
            std::vector<std::string> m_friendsOnline;
            Status m_onlineStatus;

            NetworkManager m_networkManager;
            UI m_ui;

            sf::RenderWindow* m_windowPtr;
            tgui::Gui* m_guiPtr;
    };
}

#endif // CLIENT_HPP
