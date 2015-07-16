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

    class Network
    {
    public:

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

        typedef std::uint32_t RequestId;
        const static RequestId InvalidRequest = 0;

        struct Task
        {
            RequestId id;
            enum Type
            {
                Login,
                Logout,
                Signup,
                SendMsg,
                SearchUser,
                AddFriend
            } type;
            std::chrono::steady_clock::time_point timeStarted;
        };

        struct Reply
        {
            RequestId id;
            enum Type
            {
                TaskSuccess,
                TaskError,
                TaskTimeout,
                RecievedMessage
//                Disconnected
            } type;
            std::vector<std::string> args;
        };
        // Default constructor
        Network();

        bool isConnected();

        void reset();

        /**
        Make a request to server, requests are of different types and expect different arguments
        Task type                 | Arguments required/passed
        --------------------------+--------------------------------------------------
        Login                     | Server-Address, Server-port, Username, Password
        (Creates a connection)    |
        --------------------------+--------------------------------------------------
        Signup                    | Server-Address, Server-port, Username, Password
        (One-time connection)     |
        --------------------------+--------------------------------------------------
        SendMsg                   | Recipient, Message data
        --------------------------+--------------------------------------------------
        SearchUser                | Query-name
        --------------------------+--------------------------------------------------
        AddFriend                 | Friend-username
        --------------------------+--------------------------------------------------
        Logout                    | (no args)

        @return the RequestId of the task/request

        An exception is thrown if some critical error hath eventuated (wrong number of arguments)
        Network::InvalidRequest is returned if the socket is disconnected
        **/
        RequestId send(Task::Type task, const std::vector<std::string>& args = {});

        // This looks if new packets have arrived, if so, it checks their validity,
        // parses and stores 'em in m_replies, returns an int with the number of new replies
        // This also makes a reply for tasks that have timed-out.
        int update();

        // Returns a reply (sent by the server)
        // If the reply was a confirmation/error for a task, request id is the id of the *sent* request
        // otherwise it is the request id of the *received*
        Reply popReply();

    private:
        void disconnect();

        // The default timeout period (in ms) : 10000 ms (or 10s)
        const unsigned int m_defaultTaskTimeout = 10000;

        //Stores data for either a Login or Signup task, used to connect in multiple steps
        struct
        {
            std::string address;
            unsigned short port;
            std::string username, password;
        } m_connectManifest;

        RequestId generateId();

        bool m_connected;

        //List of tasks which haven't received a reply from server
        std::list<Task> m_tasks;
        //list of replies yet to popReply()'ed
        std::list<Reply> m_replies;

        RequestId m_idCounter;

        //SFML network specific objects
        sf::TcpSocket m_socket;
    };
}
#endif // NETWORK_HPP
