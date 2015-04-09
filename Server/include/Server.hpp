/**

    Prattle/Server/Server.hpp
    =========================

    See Server.md

*/

#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>
#include <memory>
#include <SFML/Network.hpp>
#include "../include/System.hpp"
#include "../include/UserDatabase.hpp"

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

    const std::string NOTIF_LOGIN         = "notif_login";
    const std::string NOTIF_LOGOUT        = "notif_logout";
    const std::string NOTIF_ONLINE        = "notif_online";

    // Class Server which is the main component for Prattle's Server. It controls
    // the overall chat process as described in the beginning of this file.

    class Server
    {
        public:
            Server();

            void run();                         // Runs the server and has the main control loop of the program

            bool isRunning();                   // Returns true if 'm_running' is true.

            bool wait();                        // Causes 'm_selector' to wait until one or more sockets are ready to receive.
                                                // Returns true if a socket is ready.

            bool newConnectionRequest();        // Listens for new connections until a socket is ready to accept a new connection.

            void addNewClient();                // Adds a connected client (a sf::TcpSocket) to the list of currently connected
                                                // clients 'm_clients'.

            bool send(const sf::Packet& packet);// sends a packet (an instance of sf::Packet) to a connected
                                                // client (an instance of sf::TcpSocket)

            void receive();                     // Receive packets from all clients who are either connected or trying to connect.

            void shutdown();                    // Stop listening to incoming connections

        protected:
            void parseConfigFile();

        private:
            sf::TcpListener m_listener;         // Listens to incoming connections at port OPEN_PORT.
            sf::SocketSelector m_selector;      // Selector class for interacting with multiple sockets.
            std::map<std::string, std::unique_ptr<sf::TcpSocket>> m_clients;
                                                // The TcpSockets that act as clients.
            bool m_running;                     // Boolean to indicate the running state of the server.
            UserDatabase db;                    // The user database object that handles or database related tasks.
            sf::Time timeOut;                   // The timeout limit.
            std::multimap<std::string, std::pair<std::string, sf::Packet>> m_messages;
                                                // A map to store the messages being sent by online clients to offline clients.
            std::list<std::unique_ptr<sf::TcpSocket>> newConnections;
                                                // List to store the clients who have established connection
                                                // with the server but are not ready to send/receive messages yet.

            int m_server_port;

            std::ifstream m_configFile;

            //std::string m_globalChat;
    };
}

#endif // SERVER_HPP
