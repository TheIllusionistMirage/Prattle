/******************************************************************
 *                   Prattle/Server/Server.hpp                    *
 *    ========================================================    *
 *                                                                *
 *    The Server class is responsible to binding a listener to    *
 *    the desired communication port, waiting-listening for       *
 *    incoming connections and serving the connected clients      *
 *    according to a well established set of protocols as         *
 *    described in Documentation.md. It is responsible for        *
 *    handling database operations through UserDatabase.          *
 *                                                                *
 *    See https://github.com/TheIllusionistMirage/Prattle for     *
 *    more details.                                               *
 *                                                                *
 ******************************************************************/


#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>
#include <memory>
#include <SFML/Network.hpp>
#include "../include/System.hpp"
#include "../include/UserDatabase.hpp"

namespace prattle
{
    ///////////////////////////////////////////////////////////
    //                                                       //
    //  Network protocols                                    //
    //                                                       //
    //  NOTE :                                               //
    //  ---------------------------------------------------  //
    //  These protocols describe "what" is exchanged in the  //
    //  "sf::Packet" objects by the server and the clients.  //
    //  They DON'T describe the EXACT contents of the        //
    //  packets itself.                                      //
    //                                                       //
    ///////////////////////////////////////////////////////////

    const std::string SERVER                   = "server";

    const std::string LOGIN                    = "login";
    const std::string LOGIN_SUCCESS            = "login_success";
    const std::string LOGIN_FAILURE            = "login_failure";

    const std::string SIGNUP                   = "signup";
    const std::string SIGNUP_SUCCESS           = "signup_success";
    const std::string SIGNUP_FAILURE           = "signup_failure";

    const std::string SEND_MSG                 = "send_msg";
    const std::string SEND_MSG_SUCCESS         = "send_msg_success";
    const std::string SEND_MSG_FAILURE         = "send_msg_failure";

    const std::string SEARCH_USER              = "search_user";
    const std::string SEARCH_USER_RESULTS      = "search_user_results";

    const std::string ADD_FRIEND               = "add_friend"        ;
    const std::string ADD_FRIEND_REQ_ACK       = "add_friend_req_ack";
    const std::string ADD_FRIEND_REQ           = "add_friend_req"    ;
    const std::string ADD_FRIEND_ACCEPT        = "add_friend_accept" ;
    const std::string ADD_FRIEND_IGNORE        = "add_friend_ignore" ;
    const std::string ADD_FRIEND_SUCCESS       = "add_friend_success";

    const std::string STATUS_ONLINE            = "status_online";
    const std::string STATUS_OFFLINE           = "status_offline";

    const std::string S_CONTROLLER_ATTACH      = "controller_attach";
    const std::string S_CONTROLLER_SHUTDOWN    = "controller_shutdown";
    const std::string S_CONTROLLER_SHOW_USERS  = "show_logged_users";
    const std::string S_CONTROLLER_STATS       = "print_stats";
    const std::string S_CONTROLLER_REMOVE_USER = "remove_user";


    ///////////////////////////////////////////////////////////////////////////
    //                                                                       //
    //      Server class is responsible for overseeing the communication     //
    //      process of multiple clients and also controlling the database    //
    //      through UserDatabase.                                            //
    //                                                                       //
    ///////////////////////////////////////////////////////////////////////////
    //                                                                       //
    //  The server consists of a special multiplexer (sf::SocketSelector)    //
    //  that allows to check if any sockets (sf::TcpSocket/sf::TcpListener)  //
    //  are ready to receive, a listener (sf::TcpListener) that allows to    //
    //  check if new sockets are ready to receive, and a list of socket      //
    //  handles (sf::TcpSocket) that are the connected remote clients.       //
    //                                                                       //
    //  While the main loop is running, the selector waits until one or      //
    //  more sockets are ready to receive. If a socket is ready to receive,  //
    //  then using the listener, it is checked whether a new client is       //
    //  trying to connect. If yes and if there are no errors, then the       //
    //  client is added the list of clients. Else the existing clients are   //
    //  checked using the selector (if ready) and data is received form      //
    //  them.                                                                //
    //                                                                       //
    ///////////////////////////////////////////////////////////////////////////
    //                                                                       //
    //  NOTE :                                                               //
    //  -------------------------------------------------------------------  //
    //  Whenever the word `packet` is used, it used to refer to the          //
    //  high-level abstraction of packet in `sf::Packet`.                    //
    //                                                                       //
    ///////////////////////////////////////////////////////////////////////////

    class Server
    {
        public:

            // Start the main loop of the server
            void run();

            // Returns TRUE if the server is running. Else returns FALSE
            bool isRunning();

            // Gracefully release all resources and shutdown
            void shutdown();

            Server();

        private:

            // Parse server config file
            void parseConfigFile();

            // Waits until one or more sockets are
            // ready to receive. Returns TRUE if a
            // socket is ready to receive.
            bool wait();

            // Listens for new connections until a socket
            // is ready to accept a new connection.
            bool newConnectionRequest();

            // Adds a new client to the client list
            void addNewClient();

            // Send a packet to a client
            bool send(sf::Packet& packet, const std::string& username);

            // Send a packet to prattle-server-controller
            bool sendController(sf::Packet& packet);

            // Receive packets from connected clients
            void receive();

            // Process a new connection request. Protocols
            // handled are LOGIN and SIGNUP
            void handleNewConnection();

            // Process an existing connection. Protocols handled
            // are SEND_MSG, SEARCH_USER and ADD_FRIEND
            void handleClientRequest(sf::Packet& packet ,
                                     const std::string& sender);

            // Process commands received by controller
            void receiveCommand();

        private:

            // Listens to incoming connections at port OPEN_PORT.
            sf::TcpListener m_listener;

            // Selector class for interacting with multiple sockets.
            sf::SocketSelector m_selector;

            // Map to store clients, usernames are keys
            std::map<std::string, std::unique_ptr<sf::TcpSocket>> m_clients;

            // UserDatabse component to handle database related tasks.
            UserDatabase db;

            // Map to store the messages being sent by
            // online clients to offline clients.
            std::multimap<std::string, sf::Packet> m_messages;

            // Stores the friend requests
            std::map<std::string, std::vector<std::string>> m_friendReqs;

            // List to store the clients who have established connection with
            // the server but are not ready to send/receive messages yet.
            std::list<std::unique_ptr<sf::TcpSocket>> m_new_connections;

            // Handle to server controller, if attached.
            std::unique_ptr<sf::TcpSocket> m_controller;

            // Server controller passphrase hash.
            std::string m_ctrlr_pass_hash;

            // TRUE if server is up, else FALSE.
            bool m_running;

            // The port to which listener is bound to (OPEN_PORT).
            int m_server_port;

            // The default timeout period for networking tasks.
            sf::Time timeOut;

            // Time since server was started.
            sf::Clock m_clock;

            // Server config file.
            std::ifstream m_configFile;
    };
}

#endif // SERVER_HPP
