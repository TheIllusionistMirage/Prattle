#ifndef CLIENTGUI_HPP
#define CLIENTGUI_HPP

#include "../include/Client.hpp"
#include "../include/UI.hpp"

namespace prattle
{
    class ClientGUI : public Client, public UI
    {
        public:
            ClientGUI();

            void run(float dt);

            void reset();

            bool login();
            bool signup();
            void logout();
            bool searchUsername();
            bool addFriend();

        protected:

        private:


    };
}

#endif // CLIENTGUI_HPP
