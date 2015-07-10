#include "../include/Network.hpp"

namespace prattle
{
    Network::Network() : m_addr{""} ,
                         m_port{-1} ,
                         m_socket{} ,
                         m_idCounter{0},
                         m_connected{false}
    {
        //m_socket.setBlocking(false);
        m_socket.setBlocking(true);
    }

    void Network::reset()
    {
        disconnect();
        //m_socket.setBlocking(false);
        m_socket.setBlocking(true);
        m_connected = false;
    }

    bool Network::connect()
    {
        return m_socket.connect(m_addr, m_port, sf::milliseconds(DEFAULT_TIMEOUT_PERIOD)) == sf::Socket::Status::Done;
//        std::cout << m_addr << " " << m_port << std::endl;
//        sf::Socket::Status s = m_socket.connect(m_addr, m_port, sf::milliseconds(DEFAULT_TIMEOUT_PERIOD));
//
//        if (s == sf::Socket::Status::Done)
//        {
//            m_connected = true;
//            return true;
//        }
//        else if (s == sf::Socket::Status::Error)
//        {
//            std::cout << "E" << std::endl;
//            return false;
//        }
//        else if (s == sf::Socket::Status::NotReady)
//        {
//            std::cout << "N" << std::endl;
//            return false;
//        }
    }

    void Network::disconnect()
    {
        m_socket.disconnect();
    }

    void Network::setBlocking(bool blocking)
    {
        m_socket.setBlocking(blocking);
    }

    void Network::setIpAndPort(const std::string& ip, const unsigned int port)
    {
        m_addr = ip;
        m_port = port;
    }

    bool Network::isConnected()
    {
        return m_connected;
    }

    Network::RequestId Network::send(TaskType task, const std::vector<std::string>& args)
    {
        sf::Packet loginPacket;

        if (task == TaskType::Login)
            loginPacket << LOGIN;

        else if (task == TaskType::Signup)
            loginPacket << SIGNUP;

        else if (task == TaskType::SendMsg)
            loginPacket << SEND_MSG;

        else if (task == TaskType::Search)
            loginPacket << SEARCH_USER;

        else if (task == TaskType::AddFriend)
            loginPacket << ADD_FRIEND;

        for (auto& i : args)
            loginPacket << i;

        if (m_socket.send(loginPacket) == sf::Socket::Status::Done)
        {
            m_tasks.push_back(Task{m_idCounter++, task, std::chrono::steady_clock::now()});
            return m_idCounter;
        }

        return 0;
    }

    std::list<Network::Task> Network::getPendingTasks()
    {
        return m_tasks;
    }

    int Network::receive()
    {
        sf::Packet packet;

        if (m_socket.receive(packet) == sf::Socket::Status::Done)
        {
            std::string protocol, str;
            std::vector<std::string> args;

            if (packet >> protocol)
            {
                while (packet >> str)
                    args.push_back(str);

                if (protocol == LOGIN_SUCCESS || protocol == LOGIN_FAILURE)
                {
                    for (auto& j : m_tasks)
                        if (j.type == Login)
                            m_replies.push_back(Reply{j.id, (protocol == LOGIN_SUCCESS ? TaskSuccess : TaskError), args});
                }
            }
            else
                LOG("ERROR :: Damaged packet received from server.");
        }

        else if (m_socket.receive(packet) == sf::Socket::Status::Error)
            LOG("ERROR :: Unable to receive from the server.");

        for (auto i = m_tasks.begin(); i != m_tasks.end(); i++)
        {
            //if (std:: std::chrono::steady_clock::now() - (*i).timeStarted > DEFAULT_TIMEOUT_PERIOD / 1000)
            std::chrono::steady_clock::duration timeElapsed = std::chrono::steady_clock::now() - (*i).timeStarted;

            if (double(timeElapsed.count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den > DEFAULT_TIMEOUT_PERIOD / 1000)
            {
                LOG("LOG :: Task timed out. Please try again.");
                m_tasks.erase(i);
            }
        }

        return m_replies.size();
    }

    const Network::Reply Network::popReply()
    {
        Reply r = m_replies.back();
        m_replies.pop_back();
        return r;
    }
}
