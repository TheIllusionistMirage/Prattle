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
            Network::RequestId m_loginReqId = -1;
            Network::RequestId m_signupReqId = -1;
            std::vector<Network::RequestId> m_unsentMsgReqId;
            Network::RequestId m_searchReqId = -1;
            Network::RequestId m_addFriendReqId = -1;

            std::unique_ptr<UserInterface> m_ui;

            // Stores basic configuration info
            struct Configuration
            {
                std::string addr;
                int port;
                std::string ui;
            } m_clientConf;

            std::map<std::string, std::string> m_chatHistory;
    };
}

#endif // CLIENT_HPP
