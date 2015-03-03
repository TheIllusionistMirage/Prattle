/**

    Prattle/Client/NetworkManager.hpp
    =================================

    Handles packet based communication for the client.
    To communicate with fellow clients:
        > Connect to the server
        > Establish a connection for communicating (either by signing up or logging in)
        > And keep prattling

*/

#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <iostream>
#include <SFML/Network.hpp>
#include "../include/System.hpp"

namespace prattle
{
    /* Protocols */

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

    // Class NetworkManager which handles the communication
    // part of the Client application.

    class NetworkManager
    {
        public:

            NetworkManager();

            bool receive(sf::Packet& packet);       // Using the member sf::TcpSocket, listens
                                                    // to incoming packets (instances of sf::Packets)

            bool send(const sf::Packet& packet);    // Sends a packet whose destination is present
                                                    // within the packet

            void reset();                           // Reset the class objects

            void setSocketBlocking(bool blocking);  // Block the member sf::TcpSocket for all incoming packets

            bool connectToServer(const std::string serverIP,
                                 const int port);
                                                    // Establish a connection with the server

        private:

            sf::TcpSocket m_clientSocket;           // The socket class instance
    };
}

#endif // NETWORK_MANAGER_HPP
