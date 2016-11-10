/**************************************************************
 *                                                            *
 *                 Prattle/Client/Network.hpp                 *
 *    ====================================================    *
 *                                                            *
 *    The networking interface for Prattle-client. The        *
 *    Network class handles all network operations with       *
 *    the prattle-server.                                     *
 *                                                            *
 *    For a detailed description of the protocols used,       *
 *    refer to Documentation.md.                              *
 *                                                            *
 **************************************************************/


#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <cstdint>
#include <vector>
#include <list>
#include <string>
#include <chrono>

#include <SFML/Network.hpp>
#include "../include/System.hpp"

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


    ///////////////////////////////////////////////////////////////
    //                                                           //
    //  The Network class serves as an interface to the          //
    //  networking operations for the client. It is responsible  //
    //  for sending/receiving info to/from the prattle-server,   //
    //  and passing relevant info to the UserInterface class.    //
    //                                                           //
    //  By default, the client sockets are non-blocking,         //
    //  enabling the UserInterface class to do UI operations     //
    //  in a single thread and the Network class to do           //
    //  networking operations in a single thread.                //
    //                                                           //
    ///////////////////////////////////////////////////////////////

    class Network
    {
        public:

            // The task request id
            typedef std::uint32_t  RequestId;

            // By default, invalid requests have a request id of 0
            const static RequestId InvalidRequest = 0;


            // Structure Task abstracts a pending network
            // task (e.g., login, signup, etc.)
            //
            // A Task consists of:
            // - Task id
            // - Task type (login, signup, etc.)
            // - Start time
            struct Task
            {
                RequestId id;

                enum Type
                {
                    Login               ,
                    Logout              ,
                    Signup              ,
                    SendMsg             ,
                    SearchUser          ,
                    AddFriend           ,
                    FriendRequestAccept ,
                    FriendRequestIgnore
                } type;

                std::chrono::steady_clock::time_point timeStarted;
            };

            // Structure Reply abstracts a reply recevied from the server
            //
            // A Reply consists of:
            // - Reply id
            // - Reply type (succes, error, etc.)
            // - Arguments (which contain the server's reply)
            struct Reply
            {
                RequestId id;

                enum Type
                {
                    TaskSuccess        ,
                    TaskError          ,
                    TaskTimeout        ,
                    RecievedMessage    ,
                    OnlineNotif        ,
                    OfflineNotif       ,
                    FriendAdded        ,
                    FriendAddedSuccess
                } type;

                std::vector<std::string> args;
            };


        public:

            // Send a message to the prattle-server,
            // requesting a specific task
            RequestId send(Task::Type task ,
                           const std::vector<std::string>& args = {});

            // This looks if new packets have arrived, if so, it
            // checks their validity, parses and stores them in
            // m_replies, and returns the number of new replies.
            // This also creates a replies for tasks that have
            // timed-out.
            int update();

            // Returns the oldest reply (sent by the server)
            //
            // If the reply was a confirmation/error for a task,
            // request id is the id of the "sent" request
            // otherwise it is the request id of the "received"
            Reply popReply();

            // Returns TRUE if the client id connected to the
            // prattle-server. Else returns FALSE.
            bool isConnected();

            // Reset the client connection to the server.
            void reset();

                 Network();

        private:

            // Close the connection to the server. Used while LOGIN/SIGNUP.
            void disconnect();

            // Returns the request id for a newly created task.
            RequestId generateId();

            // Convert socket status to human readable string.
            const std::string convertToString(const sf::Socket::Status status) const;

            // Convert task type to human readable string.
            const std::string convertToString(const Task task) const;

        private:

            // Stores user data for LOGIN and SIGNUP tasks
            struct
            {
                std::string    address  ;  // prattle-server IP
                unsigned short port     ;  // prattle-server port
                std::string    username ;  // username
                std::string    password ;  // password
            } m_connectManifest;

            // List of pending tasks.
            std::list<Task> m_tasks;

            // List of replies waiting to be processed.
            std::list<Reply> m_replies;

            // Total pending tasks.
            RequestId m_idCounter;

            // Client socket for communication with server.
            sf::TcpSocket m_socket;

            // Set to TRUE if client is connected
            // to server, else set to FALSE.
            bool m_connected;

            // The default timeout period (in ms) : 10000 ms (or 10s)
            const unsigned int m_defaultTaskTimeout = 10000;
    };
}
#endif // NETWORK_HPP
