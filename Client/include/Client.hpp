/**

    Prattle/Client/Client.hpp
    =========================

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <SFML/Network.hpp>
#include "../include/Logger.hpp"
#include "../include/Network.hpp"
#include "../include/UserInterface.hpp"
#include "../include/GraphicalUI.hpp"

namespace prattle
{
    class Client
    {
        public:

//            enum State
//            {
//                Login,
//                Signup,
//                Connecting,
//                Chatting,
//                Exit
//            };

            // Stores basic configuration info
            struct Configuration
            {
                std::string addr;
                int port;
                std::string ui;
            };

        public:

            Client();
            void update();
            void draw();
            void run();

        private:
            void parseConfigFile();
            void doLogin();
            void doSignup();
            void sendUserMessage();
            void processServerReply();
            void changeState(UserInterface::State s);

            const std::string m_configFilePath = "resources/config/client.conf";
            UserInterface::State m_state;

            Network m_network;
            Network::RequestId m_loginReqId;
            Network::RequestId m_signupReqId;

            std::unique_ptr<UserInterface> m_ui;
            Configuration m_clientConf;
    };
}

#endif // CLIENT_HPP
