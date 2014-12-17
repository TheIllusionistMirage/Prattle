/**

    Prattle/Client/Client.hpp

    The client side application for chatting.

    Client contains two instances of Prattle/Client/NetworkManager and Prattle/Client/GUI each.
    NetworkManager handles the networking part while GUI is responsible for the graphics.

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <SFML/Graphics.hpp>
#include "NetworkManager.hpp"
#include "UI.hpp"

namespace prattle
{
    class Client
    {
        public:
            Client();

            void reset();
            void run();

        protected:

        private:
            NetworkManager m_networkManager;
            UI m_ui;

            sf::RenderWindow* m_windowPtr;
            tgui::Gui* m_guiPtr;
    };
}

#endif // CLIENT_HPP
