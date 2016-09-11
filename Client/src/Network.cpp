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
                unsigned short port = stoi(args[1]);
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
                unsigned short port = stoi(args[1]);
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
                packet << SEND_MSG << to_string(rid) << args[0] << args[1];
                //std::cout << SEND_MSG << " " + to_string(rid) << " " << args[0] << " " << args[1] << std::endl;
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
            case Task::Type::SearchUser:
            {
                DBG_LOG("Searching task added");
                if (args.size() != 1)
                    throw std::invalid_argument("Wrong number of arguments provided for task: Search");
                if (!m_tasks.empty())
                    WRN_LOG("Warning: Trying to search on a new server but the task list is not empty. It is cleared.");
                m_tasks.clear();
                if (!m_replies.empty())
                    WRN_LOG("Warning: Trying to search on a new server but the replies stack is not empty. It is cleared.");
                m_replies.clear();

                m_tasks.push_front(Task{
                                  rid = generateId(),
                                  Task::Type::SearchUser,
                                  std::chrono::steady_clock::now()});

                sf::Packet packet;
                packet << SEARCH_USER << to_string(rid) << args[0];
                //std::cout << SEND_MSG << " " + to_string(rid) << " " << args[0] << " " << args[1] << std::endl;
                auto status = m_socket.send(packet);

                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG("Search packet sent to server");
                    return rid;
                }
                else
                    ERR_LOG("ERROR in sending search packet to sever!");
            }
            break;
            case Task::Type::AddFriend:
            {
                if (args.size() != 1)
                    throw std::invalid_argument("ERROR :: Wrong number of arguments provided for task : Add Friend");

                if (!m_tasks.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the replies list is not empty. It is cleared.");

                m_replies.clear();

                m_tasks.push_front(Task{
                                  rid = generateId(),
                                  Task::Type::AddFriend,
                                  std::chrono::steady_clock::now()});

                sf::Packet packet;
                packet << ADD_FRIEND << to_string(rid) << args[0];

                auto status = m_socket.send(packet);

                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG(ADD_FRIEND + " request forwarded to server");
                    return rid;
                }
                else
                    ERR_LOG("Unable to forward " + ADD_FRIEND + " request to server");
            }
            break;

            case Task::Type::FriendRequestAccept:
            {
                if (args.size() != 1)
                    throw std::invalid_argument("ERROR :: Wrong number of arguments provided for task : Friend Request Accept");

                if (!m_tasks.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the replies list is not empty. It is cleared.");

                m_replies.clear();

                m_tasks.push_front(Task{
                                  rid = generateId(),
                                  Task::Type::FriendRequestAccept,
                                  std::chrono::steady_clock::now()});

                sf::Packet packet;
                packet << ADD_FRIEND_ACCEPT << to_string(rid) << args[0];

                auto status = m_socket.send(packet);

                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG("Friend request accept packet sent to server");
                    return rid;
                }
                else
                    ERR_LOG("ERROR in sending friend request accept packet to sever!");
            }
            break;

            case Task::Type::FriendRequestIgnore:
            {
                if (args.size() != 1)
                    throw std::invalid_argument("ERROR :: Wrong number of arguments provided for task : Friend Request ignore");

                if (!m_tasks.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the replies list is not empty. It is cleared.");

                m_replies.clear();

                m_tasks.push_front(Task{
                                  rid = generateId(),
                                  Task::Type::FriendRequestIgnore,
                                  std::chrono::steady_clock::now()});

                sf::Packet packet;
                packet << ADD_FRIEND_IGNORE << to_string(rid) << args[0];

                auto status = m_socket.send(packet);

                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG("Friend request ignore packet sent to server");
                    return rid;
                }
                else
                    ERR_LOG("ERROR in sending friend request ignore packet to sever!");
            }
            break;

            case Task::Type::Logout:
                DBG_LOG("Network :: Logging out");
                reset();
                break;
            default:
                ERR_LOG("FATAL ERROR :: Unhandled task");
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
                    reqPacket << to_string(m_tasks.front().id) << m_connectManifest.username
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
                        ERR_LOG("Error in sending request packet. Status code: " + to_string(status));
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
                    ERR_LOG("Error in connecting. Status code: " + to_string(status));
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

                        //std::cout << sender << data << std::endl;
                    }
                    else if (reply == STATUS_ONLINE || reply == STATUS_OFFLINE)
                    {
                        std::string sender;
                        response >> sender;

                        //std::cout << reply << sender << std::endl;

                        m_replies.push_front(Reply{
                                            InvalidRequest,
                                            reply == STATUS_ONLINE ? Reply::Type::OnlineNotif : Reply::Type::OfflineNotif,// Reply::Type::OnlineNotif,
                                            {sender} });
                    }
//                    else if (reply == ADD_FRIEND)
//                    {
//                        std::string sender;
//                        response >> sender;
//
//                        m_replies.push_front(Reply{
//                                            InvalidRequest,
//                                            Reply::Type::TaskSuccess,
//                                            {sender} });
//                    }
//                    else if (reply == ADD_FRIEND_REQ)
//                    {
//                        std::string sender;
//                        response >> sender;
//
//                        m_replies.push_front(Reply{
//                                            InvalidRequest,
//                                            Reply::Type::TaskSuccess,
//                                            {sender}});
//                    }


                    else if (reply == ADD_FRIEND_REQ)
                    {
                        std::string sender;
                        response >> sender;

                        m_replies.push_front(Reply{
                                            InvalidRequest,
                                            Reply::Type::FriendAdded,
                                            {sender}});
                    }

//                    else if (reply == ADD_FRIEND_SUCCESS)
//                    {
//                        std::string friendname;
//
//                        response >> friendname;
//
//                        m_replies.push_front(Reply{
//                                            InvalidRequest,
//                                            Reply::Type::FriendAdded,
//                                            {friendname}});
//
//                        DBG_LOG("Add friend task completed");
//                        //m_tasks.erase(res);
//                    }

                    else if (reply == ADD_FRIEND_SUCCESS)
                    {
                        std::string friendname;
                        response >> friendname;

                        m_replies.push_front(Reply{
                                            InvalidRequest,
                                            Reply::Type::FriendAddedSuccess,
                                            {friendname}});

                        DBG_LOG(ADD_FRIEND + " request completed");
                    }

                    else
                    {
                        if (response >> temp)
                        {
                            //std::cout << "Rid : " << temp << std::endl;
                            RequestId rid = stoi(temp);

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
                                        vec.push_back(to_string(noOfFriends));
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
                                else if (reply == SEARCH_USER_RESULTS)
                                {
                                    unsigned int noOfFriends;
                                    std::vector<std::string> result;

                                    if (response >> noOfFriends)
                                    {
                                        result.push_back(to_string(noOfFriends));
                                        while (response >> temp)
                                            result.push_back(temp);

                                        m_replies.push_front(Reply{
                                                            rid,
                                                            Reply::Type::TaskSuccess,
                                                            result});

                                        DBG_LOG("Send message task erased");
                                        m_tasks.erase(res);
                                    }
                                    else
                                        ERR_LOG("Damaged packet received");
                                }

                                else if (reply == ADD_FRIEND_REQ_ACK)
                                {
                                    std::string status, _friend;
                                    response >> status >> _friend;

                                    /*
                                        Error checking code goes here, if any
                                    */
                                    // (use the contents of status to check for errors)
                                    // for now it always contains "success"
                                    /* /// */

                                    m_replies.push_front(Reply{rid,
                                                               Reply::Type::TaskSuccess,
                                                               {_friend}});
                                    m_tasks.erase(res);

                                    DBG_LOG("Friend request to \'" + _friend + "\' sent");
                                }

//                                else if (reply == ADD_FRIEND_REQ_SUCCESS ||
//                                          reply == ADD_FRIEND_REQ_FAILURE)
//                                {
//                                    std::string result;
//                                    response >> result;
//
//                                    m_replies.push_front(Reply{
//                                                        rid,
//                                                        Reply::Type::TaskSuccess,
//                                                        {result}});
//                                    DBG_LOG("Friend request sent to \'" + result + "\'.");
//                                    m_tasks.erase(res);
//                                }
//                                else if (reply == ADD_FRIEND_SUCCESS ||
//                                          reply == ADD_FRIEND_FAILURE)
//                                {
//                                    std::string result;
//
//                                    response >> result;
//
//                                    m_replies.push_front(Reply{
//                                                        rid,
//                                                        Reply::Type::TaskSuccess,
//                                                        {result}});
//
//                                    DBG_LOG("Add friend task completed");
//                                    m_tasks.erase(res);
//                                }

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
                    DBG_LOG("LOG :: Task timed out. Request id: " + to_string(i->id) + " Time elapsed: " + to_string(std::chrono::duration_cast<std::chrono::milliseconds>(d).count()));
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
