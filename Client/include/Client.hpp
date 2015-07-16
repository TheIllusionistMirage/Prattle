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
            Client();
            void update();
            void draw();
            void run();

        private:
            void parseConfigFile();
            void changeState(UserInterface::State s);

            const std::string m_configFilePath = "resources/config/client.conf";
            UserInterface::State m_state;

            Network m_network;
            Network::RequestId m_loginReqId;
            Network::RequestId m_signupReqId;

            std::unique_ptr<UserInterface> m_ui;

            // Stores basic configuration info
            struct Configuration
            {
                std::string addr;
                int port;
                std::string ui;
            } m_clientConf;
    };
}

#endif // CLIENT_HPP
