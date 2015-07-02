/**

    Prattle/Client/Client.hpp
    =========================

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <SFML/Network.hpp>
#include "../include/ErrorLog.hpp"
#include "../include/Network.hpp"
#include "../include/UserInterface.hpp"
#include "../include/GraphicalUI.hpp"

namespace prattle
{
    // Constants
    enum class Status               // Online status
    {
        Online,
        Offline
    };

    class Client
    {
        public:

            enum State
            {
                Login,
                Signup,
                Connecting,
                Chatting,
                Exit
            };

            struct Conf      // Stores basic configuration info
            {
                std::string addr;
                int port;
                std::string ui;
            };

        public:

            Client();
            void update();
            void draw();
            void run(float);

        private:
            void parseConfigFile();
            void doLogin();
            void doSignup();
            void sendUserMessage();
            void processServerReply();
            void changeState(State s);

            const std::string m_configFilePath = "resources/config/client.conf";
            State m_state;
            Network m_network;
            std::shared_ptr<UserInterface> m_ui;
            Conf m_client_conf;
    };
}

#endif // CLIENT_HPP
