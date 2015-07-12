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
    /* Protocol requests */

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

    const std::string STATUS_ONLINE       = "status_online";
    const std::string STATUS_OFFLINE      = "status_offline";

    // Class Server which is the main component for Prattle's Server. It controls
    // the overall chat process as described in the beginning of this file.

    class Server
    {
        public:
            Server();

            void run();                         // Runs the server and has the main control loop of the program

            bool isRunning();                   // Returns true if 'm_running' is true.

        private:
            void shutdown();                    // Stop listening to incoming connections

            void parseConfigFile();

            bool wait();                        // Causes 'm_selector' to wait until one or more sockets are ready to receive.
                                                // Returns true if a socket is ready.

            bool newConnectionRequest();        // Listens for new connections until a socket is ready to accept a new connection.

            void addNewClient();                // Adds a connected client (a sf::TcpSocket) to the list of currently connected
                                                // clients 'm_clients'.

            bool send(const sf::Packet& packet, const std::string& username);// sends a packet (an instance of sf::Packet) to a connected
                                                // client (an instance of sf::TcpSocket)
            bool sendController(sf::Packet& packet); // send to controller
            void receive();                     // Receive packets from all clients who are either connected or trying to connect.
            void handleNewConnection();
            void handleClientRequest(sf::Packet& packet, const std::string& sender);
            void receiveCommand(); //Handle commands received by controller
            sf::TcpListener m_listener;         // Listens to incoming connections at port OPEN_PORT.
            sf::SocketSelector m_selector;      // Selector class for interacting with multiple sockets.
            std::map<std::string, std::unique_ptr<sf::TcpSocket>> m_clients;
                                                // The TcpSockets that act as clients.
            bool m_running;                     // Boolean to indicate the running state of the server.
            UserDatabase db;                    // The user database object that handles or database related tasks.
            sf::Time timeOut;                   // The timeout limit for waiting on packets/connections.
            std::multimap<std::string, sf::Packet> m_messages;
                                                // A map to store the messages being sent by online clients to offline clients.
            std::list<std::unique_ptr<sf::TcpSocket>> m_new_connections;
                                                // List to store the clients who have established connection
                                                // with the server but are not ready to send/receive messages yet.
            std::unique_ptr<sf::TcpSocket> m_controller;           // Server controller, if attached
            std::string m_ctrlr_pass_hash;

            int m_server_port;

            sf::Clock m_clock; // Time since server was started
            std::ifstream m_configFile;
            //std::string m_globalChat;
    };
}

#endif // SERVER_HPP
