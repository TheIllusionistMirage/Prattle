/**

    Prattle/Server/Server.hpp
    =========================

    The server which hosts the chat process.

    The server is responsible for:
     > Communication between clients (through packets)
     > User login and registration
     > Controlling what goes and does not go into the user database.

    The clients communicate through the server, i.e., all packets
    shared between clients are first received by the server and
    then sent to the intended receiver.

    So, intuitively, the chat process can be visualized as below:

    *******************************************************************


                                ============
                                | CLIENT 2 |
                                ============
                                     |
                                     |
                                     |
                                     |
                                     |
                                     |
      ============               ==========               ============
      | CLIENT 1 | ------------- | SERVER | ------------- | CLIENT 3 |
      ============               ==========               ============
                                     |
                                     |
                                     |
                                     |
                                     |
                                     |
                                ============
                                | CLIENT 4 |
                                ============


    *******************************************************************

    Protocols
    =========

    Current communication protocols followed by Prattle :

    Data-types :
    * Strings : Null-terminated ANSI strings composed of single (8-bit) byte characters
        (Comment : This could be UTF32 strings in the future)
    * Integers : 32-bit integers in the Network byte order (i.e. big endian order)

    All words separated by a space in the given packet are distinct null terminated strings

    I. Login Protocols
    ==================

    1.1 Packet sent to Server          : LOGIN username password
    1.2 Packet sent to Client as reply : LOGIN-SUCCESS NumberOfFriends friend1 friend2 ...
    1.3 Packet sent to Client as reply : LOGIN-FAILURE details


    II. Signup Protocols
    ====================

    2.1 Packet sent to Server          : SIGNUP username password
    2.2 Packet sent to Client as reply : SIGNUP-SUCCESS
    2.3 Packet sent to Client as reply : SIGNUP-FAILURE details


    III. Packet exchange between two(or more) Clients Protocols
    ===========================================================

    Packet sent to Server              : SENDMSG destinationUser Message
    Packet sent to Client as reply     : SENDMSG-ACK

    IV. Searching Database Protocols
    ================================

    Packet sent to Server              : SEARCH-USER name
    Packet sent to Client as reply     : SEARCH-USER-RESULT NumOfResults name1 name2 ...

    V. Adding friends Protocols
    ============================

    Packet sent to Server              : ADD-FRIEND name
    Packet sent to Client as reply     : ADD-FRIEND-SUCCESS
    Packet sent to Client as reply     : ADD-FRIEND-FAILURE details

*/

#ifndef SERVER_HPP
#define SERVER_HPP


#include <iostream>
#include <fstream>
#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <list>
#include <SFML/Network.hpp>
#include "System.hpp"
#include "UserDatabase.hpp"


namespace prattle
{
    // Declaration of the Server class
    class Server
    {
        public:
            Server();

            bool isRunning();                   // Returns true if 'm_running' is true.

            bool wait();                        // Causes 'm_selector' to wait until one or more sockets are ready to receive.
                                                // Returns true if a socket is ready.

            bool newConnectionRequest();        // Listens for new connections until a socket is ready to accept a new connection.

            void addNewClient();                // Adds a connected client (a sf::TcpSocket) to the list of currently connected
                                                // clients 'm_clients'.

            bool send(const std::string &senderUserName,
                      const std::string &receiverUserName,
                      const sf::Packet& dataPacket);        // Sends a 'dataPacket' (an instance of sf::Packet)
                                                            // to 'receiverUserName' (an instance of sf::TcpSocket).

            void receive();                     // Receive packets from all clients who are either connected or trying to connect.

            void shutdown();                    // Stop listening to incoming connections

            void searchDatabase(const std::string& username, const std::string& resultReceiver);
                                                // Search the database for 'username' and send the result of the search to
                                                // resultReceiver

        private:
            sf::TcpListener m_listener;         // Listens to incoming connections at port OPEN_PORT.

            sf::SocketSelector m_selector;      // Selector class for interacting with multiple sockets.

            std::map<std::string, std::unique_ptr<sf::TcpSocket>> m_clients;
                                                // The TcpSockets that act as clients.

            bool m_running;                     // Boolean to indicate the running state of the server.

            UserDatabase db;                    // The user database object that handles or database related tasks.

            sf::Time timeOut;                   // The timeout limit.

            std::multimap<std::string, std::pair<std::string,sf::Packet>> m_messages;
                                                // A map to store the messages being sent by online clients to offline clients.
            std::list<std::unique_ptr<sf::TcpSocket>> newConnections;
                                                // List to store the clients who have established connection with the server but are not ready to send/receive messages yet.
    };

    Log logger;
}

#endif // SERVER_HPP
