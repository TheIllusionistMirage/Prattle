#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <iostream>
#include <SFML/Network.hpp>
#include "System.hpp"

namespace prattle
{
    /* Protocols */

    #define SERVER               "server"

    #define LOGIN                "login"
    #define LOGIN_SUCCESS        "login_success"
    #define LOGIN_FAILURE        "login_failure"

    #define SIGNUP               "signup"
    #define SIGNUP_SUCCESS       "signup_success"
    #define SIGNUP_FAILURE       "signup_failure"

    #define SEND_MSG             "send_msg"
    #define SEND_MSG_SUCCESS     "send_msg_success"
    #define SEND_MSG_FAILURE     "send_msg_failure"

    #define SEARCH_USER          "search_user"
    #define SEARCH_USER_RESULTS  "search_user_results"

    #define ADD_FRIEND           "add_friend"
    #define ADD_FRIEND_SUCCESS   "add_friend_success"
    #define ADD_FRIEND_FAILURE   "add_friend_failure"

    class NetworkManager
    {
        public:
            NetworkManager();
            bool receive(sf::Packet& packet);
            //bool receive(std::string& message);     // <= This function here is just a temporary
            bool send(const sf::Packet& packet);
            void reset();
            void setSocketBlocking(bool blocking);
            bool connectToServer(const std::string serverIP, const int port);

        protected:

        private:
            sf::TcpSocket m_clientSocket;
    };
}

#endif // NETWORK_MANAGER_HPP
