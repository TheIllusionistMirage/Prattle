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

            enum TaskType
            {
                Login,
                Signup,
                SendMsg,
                Search,
                AddFriend
            };

            struct Task
            {
                RequestId id;
                TaskType type;
                std::chrono::steady_clock::time_point timeStarted;
            };

            enum ReplyType
            {
                TaskSuccess,
                TaskError,
                TaskTimeout,
                RecievedMessage
            };

            struct Reply
            {
                RequestId id;
                ReplyType type;
                std::vector<std::string> arguments;
            };

            // The default timeout period is 10000 ms (or 10 s)
            const unsigned int DEFAULT_TIMEOUT_PERIOD = 10000;

    public:

        // Default constructor
        Network();

        void reset();

        bool connect();

        void disconnect();

        void setBlocking(bool blocking);

        void setIpAndPort(const std::string& ip, const unsigned int port);

        bool isConnected();

        void createTask(TaskType type, std::vector<std::string> args);

        //Returns the RequestId of the task/request
        RequestId send(TaskType task, const std::vector<std::string>& args);

        // This looks if new packets have arrived, if so, it checks their validity,
        // parses and stores 'em in m_replies, returns an int with the number of new replies
        // This also makes a reply for tasks that have timed-out.
        int receive();

        // Returns a reply (sent by the server)
        // If the reply was a confirmation/error for a task, request id is the id of the *sent* request
        // otherwise it is the request id of the *received*
        const Reply popReply();

        const Task popTask();

        const Task removeTask(RequestId taskId);

    private:

        //List of tasks which haven't received a reply from server
        std::list<Task> m_tasks;
        //list of replies yet to popReply()'ed
        std::list<Reply> m_replies;

        RequestId m_idCounter;

        //Location of server; could be a hostname or ip
        std::string m_addr;
        int m_port;

        //SFML network specific objects
        sf::TcpSocket m_socket;

        bool m_connected;
    };
}
#endif // NETWORK_HPP
