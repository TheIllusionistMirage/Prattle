#include "../include/Network.hpp"
#include <cassert>

namespace prattle
{
    Network::Network() :
        m_connected(false),
        m_idCounter{0}
    {
        m_socket.setBlocking(false);
    }

    void Network::reset()
    {
        m_socket.disconnect();
        m_tasks.clear();
        m_replies.clear();
        m_connected = false;
        m_idCounter = 0;
    }
    Network::RequestId Network::generateId()
    {
        return ++m_idCounter;
    }
    Network::RequestId Network::send(Task::Type task, const std::vector<std::string>& args)
    {
        //If task is not Login or Signup and the socket is disconnected, return InvalidRequest
        if(task != Task::Type::Login && task != Task::Type::Signup && m_socket.getRemotePort() == 0)
            return InvalidRequest;

        RequestId rid = InvalidRequest; //initialize to InvalidRequest

        switch(task)
        {
            case Task::Type::Login:
            {
                DBG_LOG("Task Login added.");
                if (args.size() != 4)
                    throw std::invalid_argument("Wrong number of arguments provided for task: Login");
                if (!m_tasks.empty())
                    WRN_LOG("Warning: Trying to login to a new server but the task list is not empty. It is cleared.");
                m_tasks.clear();
                if (!m_replies.empty())
                    WRN_LOG("Warning: Trying to login to a new server but the replies stack is not empty. It is cleared.");
                m_replies.clear();

                m_tasks.push_front(Task{
                                  rid = generateId(),
                                  Task::Type::Login,
                                  std::chrono::steady_clock::now()});
                unsigned short port = std::stoi(args[1]);
                m_connectManifest.address = args[0];
                m_connectManifest.port = port;
                m_connectManifest.username = args[2];
                m_connectManifest.password = args[3];
            }
            break;
            case Task::Type::Signup:
            {
                DBG_LOG("Task Signup added.");
                if (args.size() != 4)
                    throw std::invalid_argument("Wrong number of arguments provided for task: Signup");
                if (!m_tasks.empty())
                    WRN_LOG("Warning: Trying to signup on a new server but the task list is not empty. It is cleared.");
                m_tasks.clear();
                if (!m_replies.empty())
                    WRN_LOG("Warning: Trying to signup on a new server but the replies stack is not empty. It is cleared.");
                m_replies.clear();

                m_tasks.push_front(Task{
                                  rid = generateId(),
                                  Task::Type::Signup,
                                  std::chrono::steady_clock::now()});
                unsigned short port = std::stoi(args[1]);
                m_connectManifest.address = args[0];
                m_connectManifest.port = port;
                m_connectManifest.username = args[2];
                m_connectManifest.password = args[3];
            }
            break;
            case Task::Type::SendMsg:
            {
                DBG_LOG("Message Sending task added");
                if (args.size() != 2)
                    throw std::invalid_argument("Wrong number of arguments provided for task: Signup");
                if (!m_tasks.empty())
                    WRN_LOG("Warning: Trying to signup on a new server but the task list is not empty. It is cleared.");
                m_tasks.clear();
                if (!m_replies.empty())
                    WRN_LOG("Warning: Trying to signup on a new server but the replies stack is not empty. It is cleared.");
                m_replies.clear();

                m_tasks.push_front(Task{
                                  rid = generateId(),
                                  Task::Type::SendMsg,
                                  std::chrono::steady_clock::now()});

                sf::Packet packet;
                packet << SEND_MSG << std::to_string(rid) << args[0] << args[1];
                //std::cout << SEND_MSG << " " + std::to_string(rid) << " " << args[0] << " " << args[1] << std::endl;
                auto status = m_socket.send(packet);

                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG("Message packet sent to server");
                    return rid;
                }
                else
                    ERR_LOG("ERROR in sending message packet to sever!");
            }
            break;
            case Task::Type::Logout:
                DBG_LOG("Network: Logging out");
                reset();
                break;
            default:
                ERR_LOG("Unhandled task");
                break;
        }
        return rid;
    }

    bool Network::isConnected()
    {
        return m_connected;
    }

    void Network::disconnect()
    {
        m_socket.disconnect();
        m_connected = false;
    }

    int Network::update()
    {
        if (!isConnected())
        {
            //if either login or signup is a task
            if (/*m_tasks.size() == 1 &&*/
                (m_tasks.front().type == Task::Login || m_tasks.front().type == Task::Signup))
            {
                auto status = m_socket.connect(m_connectManifest.address, m_connectManifest.port);
                if (status == sf::Socket::Done)
                {
                    DBG_LOG("Connected to server");
                    sf::Packet reqPacket;
                    if (m_tasks.front().type == Task::Login)
                    {
                        reqPacket << LOGIN;
                    }
                    else
                        reqPacket << SIGNUP;
                    reqPacket << std::to_string(m_tasks.front().id) << m_connectManifest.username
                              << m_connectManifest.password;

                    //Try sending the login/signup request
                    int tries = 5;
                    do
                    {
                        status = m_socket.send(reqPacket);
                        --tries;
                    } while (status == sf::Socket::Partial && tries >= 0);

                    if (status == sf::Socket::Done)
                    {
                        m_connected = true;
                    }
                    else
                    {
                        ERR_LOG("Error in sending request packet. Status code: " + std::to_string(status));
                        m_replies.push_front(Reply{
                                            m_tasks.front().id,
                                            Reply::Type::TaskError,
                                            {} });
                        m_tasks.clear();
                        disconnect();
                    }
                }
                else if (status == sf::Socket::Error)
                {
                    ERR_LOG("Error in connecting. Status code: " + std::to_string(status));
                    m_replies.push_front(Reply{
                                        m_tasks.front().id,
                                        Reply::Type::TaskError,
                                        {} });
                    m_tasks.clear();
                    DBG_LOG("All tasks cleared");
                    disconnect();
                }
                else if (status == sf::Socket::Disconnected)
                {
                    ERR_LOG("Prematurely disconnected.");
                    m_replies.push_front(Reply{
                                        m_tasks.front().id,
                                        Reply::Type::TaskError,
                                        {} });
                    m_tasks.clear();
                    DBG_LOG("All tasks cleared");
                    disconnect();
                }
            }
            assert(m_tasks.size() <= 1);
        }
        else //connected
        {
            sf::Packet response;
            auto status = m_socket.receive(response);
            if (status == sf::Socket::Done)
            {
                std::string reply, temp;
                if (response >> reply)
                {
                    //std::cout << "Protocol : " << reply << std::endl;
                    if (reply == SEND_MSG)
                    {
                        std::string sender, data;
                        response >> sender >> data;

                        m_replies.push_front(Reply{
                                            InvalidRequest,
                                            Reply::Type::RecievedMessage,
                                            {sender, data} });

                        std::cout << sender << data << std::endl;
                    }
//                    else if (reply == STATUS_ONLINE || reply == STATUS_OFFLINE)
//                    {
//                        std::string sender;
//                        response >> sender;
//
//                        //std::cout << reply << sender << std::endl;
//
//                        m_replies.push_front(Reply{
//                                            InvalidRequest,
//                                            Reply::Type::OnlineNotif,
//                                            {sender} });
//                    }
                    else
                    {
                        if (response >> temp)
                        {
                            //std::cout << "Rid : " << temp << std::endl;
                            RequestId rid = std::stoi(temp);

                            const auto comparator = [&](const Task& t) { return t.id == rid; };
                            auto res = std::find_if(m_tasks.begin(), m_tasks.end(), comparator);
                            if (res == m_tasks.end())
                            {
                                ERR_LOG("Invalid response from server");
                            }
                            else
                            {
                                if (reply == LOGIN_FAILURE || reply == LOGIN_SUCCESS)
                                {
                                    if (reply == LOGIN_FAILURE)
                                        DBG_LOG("LOGIN_FILAURE reply recceived from Server");

                                    m_replies.push_front(Reply{
                                                        rid,
                                                        (reply == LOGIN_SUCCESS) ? Reply::Type::TaskSuccess : Reply::Type::TaskError,
                                                        {} });

                                    if (reply == LOGIN_SUCCESS)
                                    {
                                        auto& vec = m_replies.front().args;
                                        int noOfFriends;
                                        response >> noOfFriends;
                                        vec.push_back(std::to_string(noOfFriends));
                                        while (response >> temp)
                                            vec.push_back(temp);
                                    }

                                    DBG_LOG("Login task erased");
                                    m_tasks.erase(res);
                                }
                                else if (reply == SIGNUP_SUCCESS || reply == SIGNUP_FAILURE)
                                {
                                    m_replies.push_front(Reply{
                                                        rid,
                                                        (reply == SIGNUP_SUCCESS) ? Reply::Type::TaskSuccess : Reply::Type::TaskError,
                                                        {} });

                                    DBG_LOG("Signup task erased");
                                    m_tasks.erase(res);
                                    disconnect();
                                }
                                else if (reply == SEND_MSG_SUCCESS || reply == SEND_MSG_FAILURE)
                                {
                                    DBG_LOG("Message sent successfully");
                                    m_replies.push_front(Reply{
                                                        rid,
                                                        (reply == SEND_MSG_SUCCESS) ? Reply::Type::TaskSuccess : Reply::Type::TaskError,
                                                        {} });

                                    DBG_LOG("Send message task erased");
                                    m_tasks.erase(res);
                                }
                                else
                                    ERR_LOG("Unrecognized reply. (either not implemented yet or it is invalid)");
                            }
                        }
                    }
                }
            }
            else if (status == sf::Socket::Error)
            {
                ERR_LOG("Error while receiving from socket.");
            }
            else if (status == sf::Socket::Disconnected)
            {
                DBG_LOG("Disconnected.");
                disconnect();
            }

            //Check expired tasks
            for (auto i = m_tasks.begin(); i != m_tasks.end(); i++)
            {
                auto d = std::chrono::steady_clock::now() - i->timeStarted;
                if (std::chrono::duration_cast<std::chrono::milliseconds>(d).count() > m_defaultTaskTimeout)
                {
                    DBG_LOG("LOG :: Task timed out. Request id: " + std::to_string(i->id) + " Time elapsed: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(d).count()));
                    m_replies.push_front(Reply{
                                        i->id,
                                        Reply::Type::TaskTimeout,
                                        {} });
                    i = std::prev(m_tasks.erase(i));
                }
            }
        }

        return m_replies.size();
    }

    Network::Reply Network::popReply()
    {
        Reply r = m_replies.back();
        m_replies.pop_back();
        return r;
    }

}
