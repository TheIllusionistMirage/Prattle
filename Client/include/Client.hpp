/**

    Prattle/Client/Client.hpp
    =========================

    Base class for the client application of Prattle.

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <SFML/Network.hpp>
#include "../include/ErrorLog.hpp"

namespace prattle
{
    // Constants
    const std::string CONFIG_FILE = "resources/config/client.conf";

    // Protocols
    const std::string SERVER              = "server";

    const std::string LOGIN               = "login";
    const std::string LOGIN_SUCCESS       = "login_success";
    const std::string LOGIN_FAILURE       = "login_failure";

    const std::string SIGNUP              = "signup";
    const std::string SIGNUP_SUCCESS      = "signup_success";
    const std::string SIGNUP_FAILURE      = "signup_failure";

    const std::string SEND_MSG            = "send_msg";
    const std::string SEND_MSG_SUCCESS    = "send_msg_success";
    const std::string SEND_MSG_FAILURE    = "send_msg_failure";

    const std::string SEARCH_USER         = "search_user";
    const std::string SEARCH_USER_RESULTS = "search_user_results";

    const std::string ADD_FRIEND          = "add_friend";
    const std::string ADD_FRIEND_SUCCESS  = "add_friend_success";
    const std::string ADD_FRIEND_FAILURE  = "add_friend_failure";

    enum class Status               // Online status
    {
        Online,
        Away,
        Offline
    };

    class Client
    {
        public:
            Client();

            void readConfigFile();

            virtual void run(float dt) = 0;

            //const std::string& getUsername() const;
            //const std::string& getPassword() const;

            void resetSocket();             // Reset the client to a default state

            bool checkIfWhitespace(const std::string& message);

            void setStatus(Status status);
            Status getStatus();

            virtual bool login() = 0;             // Log in to the Prattle server
            virtual bool signup() = 0;            // Sign up in the Prattle server
            virtual void logout() = 0;            // Log out from the Prattle server
            virtual bool searchUsername() = 0;    // Search for a user on the Prattle server
            virtual bool addFriend() = 0;         // Add a friend from the Prattle server

            bool send(const sf::Packet& packet);    // Send a packet to the server
            bool receive(sf::Packet& packet);  // Receive a packet from the server
            bool isLoggedIn();

            void blockSocket(bool blocking);   // Set m_socket blocking/non-blocking

            void disconnect();        // Disconnects to Prattle's server
            bool connect();           // Connects to Prattle's server

        protected:

            std::string m_username;   // Store the username of the client
            std::string m_password;   // Store the password of the client
            std::vector<std::string> m_friends;     // Store the names of friends

        private:

            Status m_status;

            sf::TcpSocket m_socket;   // Client socket

            struct configuration      // Stores basic configuration info
            {
                std::string ip;
                int port;
            } m_clientConf;

            //std::string m_ip;         // Server's IP address
            //int m_port;               // Server's open port
    };
}

#endif // CLIENT_HPP
