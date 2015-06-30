/**

    Prattle/Client/Client.hpp
    =========================

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <SFML/Network.hpp>
#include "../include/ErrorLog.hpp"

namespace prattle
{
    class Client
    {
        public:
            enum State
            {
                LoginPrompt,
                Connecting,
                ChatHome,
                ErrorState,
                Exit,
            };
            Client();
            void run();

        private:
            void parseConifgFile();
            void doLogin();
            void doSignup();
            void sendUserMessage();
            void processServerReply();
            void changeState(State s);

            const std::string m_configFilePath = "resources/config/client.conf";
            State m_state;
            Network m_network;
            std::unique_ptr<UserInterface> ui;
    };
}

#endif // CLIENT_HPP
