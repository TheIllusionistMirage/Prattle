#ifndef NETWORK_H
#define NETWORK_H
#include <cstdint>
#include <vector>
#include <list>
#include <string>
#include <chrono>

#include <SFML/Network.hpp>

namespace prattle
{

    class Network
    {
        public:
        
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

    public:
        
        //Returns the RequestId of the task/request
        RequestId send(TaskType task, const std::vector<std::string>& args);

        // This looks if new packets have arrived, if so, it checks their validity,
        // parses and stores 'em in m_replies, returns an int with the number of new replies
        // This also makes a reply for tasks that have timed-out.
        int receive();

        // Returns a reply (sent by the server)
        // If the reply was a confirmation/error for a task, request id is the id of the *sent* request
        // otherwise it is the request id of the *received*
        const Reply& popReply();

    private:
        
        //List of tasks which haven't received a reply from server
        std::list<Task> m_tasks;
        //list of replies yet to popReply()'ed
        std::list<Reply> m_replies;

        //Location of server; could be a hostname or ip
        std::string m_addr;
        int m_port;

        //SFML network specific objects
        sf::TcpSocket m_socket;
    };
}
#endif // NETWORK_H
