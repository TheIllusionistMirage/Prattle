/**************************************************************
 *                                                            *
 *                 Prattle/Client/Network.hpp                 *
 *    ====================================================    *
 *                                                            *
 *    Contains implementation details of class Network        *
 *    described in Prattle/Client/Network.hpp.                *
 *    Network class handles all network operations with       *
 *    the prattle-server.                                     *
 *                                                            *
 *    See https://github.com/TheIllusionistMirage/Prattle     *
 *    for more details.                                       *
 *                                                            *
 **************************************************************/


#include "../include/Network.hpp"
#include <cassert>

namespace prattle
{
    Network::RequestId Network::send(Task::Type task ,
                                     const std::vector<std::string>& args)
    {
        // If task is not Login or Signup and the socket
        // is disconnected, return InvalidRequest.
        if(task != Task::Type::Login &&
            task != Task::Type::Signup &&
             m_socket.getRemotePort() == 0)
            return InvalidRequest;

        // Initialize to InvalidRequest.
        RequestId rid = InvalidRequest;

        // Determine the task type and construct corresponding
        // task entry and add it to the pending task list.
        switch(task)
        {

            //////////////////////////////////////
            //                                  //
            //            LOGIN task            //
            //                                  //
            //////////////////////////////////////

            case Task::Type::Login:
            {
                // If invalid info is passed
                if (args.size() != 4)
                    throw std::invalid_argument("FATAL ERROR :: Wrong number of arguments provided for task LOGIN");

                if (!m_tasks.empty())
                    WRN_LOG("Trying to login to a new server but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("Trying to login to a new server but the replies stack is not empty. It is cleared.");

                m_replies.clear();

                // Create the LOGIN task and add it to the pending task list.
                m_tasks.push_front(Task{
                                  rid = generateId() ,
                                  Task::Type::Login  ,
                                  std::chrono::steady_clock::now()});

                // Create the connection manifest to store
                // login info fetched from UserInterface.
                unsigned short port        = stoi(args[1]);
                m_connectManifest.address  = args[0];
                m_connectManifest.port     = port;
                m_connectManifest.username = args[2];
                m_connectManifest.password = args[3];

                DBG_LOG("LOGIN Task created.");
            }
            break;


            //////////////////////////////////////
            //                                  //
            //           SIGNUP task            //
            //                                  //
            //////////////////////////////////////

            case Task::Type::Signup:
            {
                // If invalid info is passed.
                if (args.size() != 4)
                    throw std::invalid_argument("Wrong number of arguments provided for task: Signup");

                if (!m_tasks.empty())
                    WRN_LOG("Trying to signup on a new server but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("Trying to signup on a new server but the replies stack is not empty. It is cleared.");

                m_replies.clear();

                // Create SIGNUP task and add it to the pending task list.
                m_tasks.push_front(Task{
                                  rid = generateId() ,
                                  Task::Type::Signup ,
                                  std::chrono::steady_clock::now()});

                // Create the connection manifest to store
                // signup info fetched from UserInterface.
                unsigned short port        = stoi(args[1]);
                m_connectManifest.address  = args[0];
                m_connectManifest.port     = port;
                m_connectManifest.username = args[2];
                m_connectManifest.password = args[3];

                DBG_LOG("SIGNUP task created.");
            }
            break;


            //////////////////////////////////////
            //                                  //
            //          SEND_MSG task           //
            //                                  //
            //////////////////////////////////////

            case Task::Type::SendMsg:
            {
                // If invalied arguments are passed.
                if (args.size() != 2)
                    throw std::invalid_argument("Wrong number of arguments provided for task: Signup");

                if (!m_tasks.empty())
                    WRN_LOG("Trying to signup on a new server but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("Trying to signup on a new server but the replies stack is not empty. It is cleared.");

                m_replies.clear();

                // Create a SEND_MSG task and add it to the pending task list.
                m_tasks.push_front(Task{
                                  rid = generateId()  ,
                                  Task::Type::SendMsg ,
                                  std::chrono::steady_clock::now()});

                DBG_LOG("SEND_MSG task created.");

                ////////////////////////////////////////////////////////////
                //                                                        //
                // Create a high level packet as per Prattle's networking //
                // protocols.                                             //
                //                                                        //
                // (Documentation.md, section 3.1 of Server-Client        //
                // protocol)                                              //
                //                                                        //
                // sendmsg <req id> <receiver> <data>                     //
                //                                                        //
                ////////////////////////////////////////////////////////////

                sf::Packet packet;
                packet << SEND_MSG << to_string(rid) << args[0] << args[1];

                // Send the packet to the server.
                auto status = m_socket.send(packet);

                // If successful in sending packet, return
                // the new request id of this task.
                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG("SEND_MSG request sent to server.");
                    return rid;
                }
                else
                    ERR_LOG("Unable to send SEND_MSG request to server.");
            }
            break;


            //////////////////////////////////////
            //                                  //
            //         SEARCH_USER task         //
            //                                  //
            //////////////////////////////////////

            case Task::Type::SearchUser:
            {
                // Check if invalid info is passed
                if (args.size() != 1)
                    throw std::invalid_argument("Wrong number of arguments provided for task: Search");

                if (!m_tasks.empty())
                    WRN_LOG("Warning: Trying to search on a new server but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("Warning: Trying to search on a new server but the replies stack is not empty. It is cleared.");

                m_replies.clear();

                // Create SEARCH_USER task and add it to the pending task list.
                m_tasks.push_front(Task{
                                  rid = generateId()     ,
                                  Task::Type::SearchUser ,
                                  std::chrono::steady_clock::now()});

                DBG_LOG("SEARCH_USER task created.");

                ////////////////////////////////////////////////////////////
                //                                                        //
                // Create a high level packet as per Prattle's networking //
                // protocols.                                             //
                //                                                        //
                // (Documentation.md, section 4.1 of Server-Client        //
                // protocol)                                              //
                //                                                        //
                // search_user <req id> <name>                            //
                //                                                        //
                ////////////////////////////////////////////////////////////

                sf::Packet packet;
                packet << SEARCH_USER << to_string(rid) << args[0];

                auto status = m_socket.send(packet);

                // If successful in sending packet, return
                // the new request id of this task.
                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG("Search packet sent to server");
                    return rid;
                }
                else
                    ERR_LOG("ERROR in sending search packet to sever!");
            }
            break;


            //////////////////////////////////////
            //                                  //
            //         ADD_FRIEND task          //
            //                                  //
            //////////////////////////////////////

            case Task::Type::AddFriend:
            {
                // If invalid info is passed
                if (args.size() != 1)
                    throw std::invalid_argument("ERROR :: Wrong number of arguments provided for task : Add Friend");

                if (!m_tasks.empty())
                    WRN_LOG("Trying to initiate a add friend task but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("Trying to initiate a add friend task but the replies list is not empty. It is cleared.");

                m_replies.clear();

                // Create a ADD_FRIEND task and add it to the pending task list.
                m_tasks.push_front(Task{
                                  rid = generateId()    ,
                                  Task::Type::AddFriend ,
                                  std::chrono::steady_clock::now()});

                DBG_LOG("ADD_FRIEND task created.");

                ////////////////////////////////////////////////////////////
                //                                                        //
                // Create a high level packet as per Prattle's networking //
                // protocols.                                             //
                //                                                        //
                // (Documentation.md, section 5.1.1 of Server-Client      //
                // protocol)                                              //
                //                                                        //
                // add_friend <req id> <friendname>                       //
                //                                                        //
                ////////////////////////////////////////////////////////////

                sf::Packet packet;
                packet << ADD_FRIEND << to_string(rid) << args[0];

                auto status = m_socket.send(packet);

                // If successful in sending packet, return
                // the request id of this task.
                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG(ADD_FRIEND + " request forwarded to server");
                    return rid;
                }
                else
                    ERR_LOG("Unable to forward " + ADD_FRIEND + " request to server");
            }
            break;

            //////////////////////////////////////
            //                                  //
            //     FRIEND_REQUST_ACCEPT task    //
            //                                  //
            //////////////////////////////////////

            case Task::Type::FriendRequestAccept:
            {
                // If invalid info is passed.
                if (args.size() != 1)
                    throw std::invalid_argument("ERROR :: Wrong number of arguments provided for task : Friend Request Accept");

                if (!m_tasks.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the replies list is not empty. It is cleared.");

                m_replies.clear();

                // Create a FRIEND_REQUEST_ACCEPT task
                // and add it to the pending task list.
                m_tasks.push_front(Task{
                                  rid = generateId()              ,
                                  Task::Type::FriendRequestAccept ,
                                  std::chrono::steady_clock::now()});

                DBG_LOG("FRIEND_REQUEST_ACCEPT task created.");

                ////////////////////////////////////////////////////////////
                //                                                        //
                // Create a high level packet as per Prattle's networking //
                // protocols.                                             //
                //                                                        //
                // (Documentation.md, section 5.2.2 of Server-Client      //
                // protocol)                                              //
                //                                                        //
                // add_friend_accept <reqid> <sender>                     //
                //                                                        //
                ////////////////////////////////////////////////////////////

                sf::Packet packet;
                packet << ADD_FRIEND_ACCEPT << to_string(rid) << args[0];

                auto status = m_socket.send(packet);

                // If successful in sending packet,
                // return the request id of this task.
                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG("Friend request accept packet sent to server");
                    return rid;
                }
                else
                    ERR_LOG("ERROR in sending friend request accept packet to sever!");
            }
            break;


            //////////////////////////////////////
            //                                  //
            //     FRIEND_REQUST_IGNORE task    //
            //                                  //
            //////////////////////////////////////

            case Task::Type::FriendRequestIgnore:
            {
                // If invalid info is passed.
                if (args.size() != 1)
                    throw std::invalid_argument("ERROR :: Wrong number of arguments provided for task : Friend Request ignore");

                if (!m_tasks.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the task list is not empty. It is cleared.");

                m_tasks.clear();

                if (!m_replies.empty())
                    WRN_LOG("WARNING :: Trying to initiate a add friend task but the replies list is not empty. It is cleared.");

                m_replies.clear();

                // Create a DRIEND_REQUEST_IGNORE task
                // and add it to the pending task list.
                m_tasks.push_front(Task{
                                  rid = generateId()             ,
                                  Task::Type::FriendRequestIgnore ,
                                  std::chrono::steady_clock::now()});


                ////////////////////////////////////////////////////////////
                //                                                        //
                // Create a high level packet as per Prattle's networking //
                // protocols.                                             //
                //                                                        //
                // (Documentation.md, section 5.2.3 of Server-Client      //
                // protocol)                                              //
                //                                                        //
                // add_friend_ignore <reqid> <sender>                     //
                //                                                        //
                ////////////////////////////////////////////////////////////
                sf::Packet packet;
                packet << ADD_FRIEND_IGNORE << to_string(rid) << args[0];

                auto status = m_socket.send(packet);

                // If successful in sending packet,
                // return the request id of this task.
                if(status == sf::Socket::Status::Done)
                {
                    DBG_LOG("Friend request ignore packet sent to server");
                    return rid;
                }
                else
                    ERR_LOG("ERROR in sending friend request ignore packet to sever!");
            }
            break;


            //////////////////////////////////////
            //                                  //
            //            LOGOUT task           //
            //                                  //
            //////////////////////////////////////

            case Task::Type::Logout:
            {
                DBG_LOG("Client logging out.");
                reset();
            }
            break;

            default:
                ERR_LOG("FATAL ERROR :: Unhandled task");
                break;
        }

        return rid;
    }

    int Network::update()
    {
        // Updation of pending tasks is done by subcategorizing pending
        // tasks into two - tasks in disconnected state (LOGIN, SIGNUP)
        // and tasks in connected state (SEND_MSG, SEARCH_USER, ADD_FRIEND,
        // FRIEND_REQUEST_ACCEPT and FRIEND_REQUEST_IGNORE).

        if (!isConnected())
        {
            if (!m_tasks.empty() &&
                (m_tasks.front().type == Task::Login ||
                  m_tasks.front().type == Task::Signup))
            {
                // Connection attempt this update iteration
                auto status = m_socket.connect(m_connectManifest.address ,
                                               m_connectManifest.port);

                // Flag `isTimedOut` is set if this task has timed out.
                std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
                bool isTimedOut =
                    std::chrono::duration_cast<std::chrono::milliseconds>
                        (t1 - m_tasks.front().timeStarted).count() >
                            m_defaultTaskTimeout;

                // If the socket has status sf::Socket::Done,
                // client socket is connected to server. The
                // pending LOGIN/SIGNUP task may be processed.
                //
                // If any the socket status is anything else and the
                // LOGIN/SIGNUP task also timed out, it's an error.
                if (status == sf::Socket::Done)
                {
                    DBG_LOG("Client socket connected to server.");

                    /////////////////////////////////////////////////
                    //                                             //
                    // Create a high level packet as per Prattle's //
                    // networking protocols.                       //
                    //                                             //
                    // (Documentation.md, section 1.1 of           //
                    // Server-Client protocol)                     //
                    //                                             //
                    // login <req id> <username> <password>        //
                    //                                             //
                    /////////////////////////////////////////////////

                    sf::Packet reqPacket;

                    if (m_tasks.front().type == Task::Login)
                        reqPacket << LOGIN;
                    else
                        reqPacket << SIGNUP;

                    reqPacket << to_string(m_tasks.front().id)
                              << m_connectManifest.username
                              << m_connectManifest.password;

                    // Try sending the login/signup request
                    int tries = 5;

                    do
                    {
                        status = m_socket.send(reqPacket);
                        --tries;
                    } while (status == sf::Socket::Partial && tries >= 0);

                    if (status == sf::Socket::Done)
                        m_connected = true;
                    else
                    {
                        ERR_LOG("Unable to send LOGIN/SIGNUP request to server. Socket status : " +
                                convertToString(status));

                        // Create a new reply denoting LOGIN/SIGNUP task error
                        m_replies.push_front(Reply{
                                            m_tasks.front().id     ,
                                            Reply::Type::TaskError ,
                                            {} });
                        m_tasks.clear();
                        DBG_LOG("All pending tasks cleared.");
                        disconnect();
                    }
                }

                else if ((status == sf::Socket::NotReady     ||
                           status == sf::Socket::Partial      ||
                            status == sf::Socket::Disconnected ||
                             status == sf::Socket::Error) &&
                           isTimedOut)
                {
                    ERR_LOG("Unable to send LOGIN/SIGNUP request to server. Socket status : " +
                            convertToString(status));

                    // Create a new reply denoting LOGIN/SIGNUP task error
                    m_replies.push_front(Reply{
                                        m_tasks.front().id     ,
                                        Reply::Type::TaskError ,
                                        {} });

                    m_tasks.clear();
                    DBG_LOG("All pending tasks cleared.");
                    disconnect();
                }

//                else if (status == sf::Socket::Partial && isTimedOut)
//                {
//                    ERR_LOG("Unable to send full data, partial data sent. Please retry");
//                    m_replies.push_front(Reply{
//                                        m_tasks.front().id,
//                                        Reply::Type::TaskError,
//                                        {} });
//
//                    m_tasks.clear();
//                    DBG_LOG("All tasks cleared");
//                    disconnect();
//                }
//
//                else if (status == sf::Socket::Disconnected)
//                {
//                    ERR_LOG("Prematurely disconnected.");
//                    m_replies.push_front(Reply{
//                                        m_tasks.front().id,
//                                        Reply::Type::TaskError,
//                                        {} });
//                    m_tasks.clear();
//                    DBG_LOG("All tasks cleared");
//                    disconnect();
//                }
//
//                else if (status == sf::Socket::Error && isTimedOut)
//                {
//                    ERR_LOG("Error in connecting. Status code: " +
//                            to_string(status));
//
//                    m_replies.push_front(Reply{
//                                        m_tasks.front().id     ,
//                                        Reply::Type::TaskError ,
//                                        {} });
//
//                    m_tasks.clear();
//                    DBG_LOG("All tasks cleared");
//                    disconnect();
//                }
//            }
//            assert(m_tasks.size() <= 1);
            }
        }
        else // connected
        {
            sf::Packet response;
            auto status = m_socket.receive(response);

            if (status == sf::Socket::Done)
            {
                std::string reply, temp;
                if (response >> reply)
                {
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

                    else if (reply == ADD_FRIEND_REQ)
                    {
                        std::string sender;
                        response >> sender;

                        m_replies.push_front(Reply{
                                            InvalidRequest,
                                            Reply::Type::FriendAdded,
                                            {sender}});
                    }

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
        }

        // Check if any task has expired and remove
        // them from the pending task list.
        for (auto i = m_tasks.begin(); i != m_tasks.end(); i++)
        {
            auto d = std::chrono::steady_clock::now() - i->timeStarted;

            if (std::chrono::duration_cast<std::chrono::milliseconds>(d).count() >
                m_defaultTaskTimeout)
            {
                DBG_LOG(" Task timed out. Request id: " + to_string(i->id) + " Time elapsed: " + to_string(std::chrono::duration_cast<std::chrono::milliseconds>(d).count()));

                    m_replies.push_front(Reply{
                                    i->id,
                                    Reply::Type::TaskTimeout,
                                    {} });
                i = std::prev(m_tasks.erase(i));
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

    bool Network::isConnected()
    {
        return m_connected;
    }

    void Network::reset()
    {
        // Disconnect from server.
        m_socket.disconnect();

        // Discard pending tasks.
        m_tasks.clear();

        // Discard unprocessed replies.
        m_replies.clear();

        // Reset connection state.
        m_connected = false;

        // Reset request id counter.
        m_idCounter = 0;
    }

    Network::Network() : m_idCounter{0} ,
                         m_connected(false)
    {
        m_socket.setBlocking(false);
    }

    void Network::disconnect()
    {
        m_socket.disconnect();
        m_connected = false;
    }

    Network::RequestId Network::generateId()
    {
        // A new id is generated jut by incrementing the id counter.
        return ++m_idCounter;
    }

    const std::string Network::convertToString(const sf::Socket::Status status) const
    {
        switch (status)
        {
            case sf::Socket::Status::Done        :  return "Done";
            case sf::Socket::Status::NotReady    :  return "NotReady";
            case sf::Socket::Status::Partial     :  return "Partial";
            case sf::Socket::Status::Disconnected:  return "Disconnected";
            case sf::Socket::Status::Error       :  return "Error";
            default                              :  return "Unknown status";
        }
    }

    const std::string Network::convertToString(const Task task) const
    {
        switch (task.type)
        {
            case Task::Type::Login              : return "LOGIN";
            case Task::Type::Logout             : return "LOGOUT";
            case Task::Type::Signup             : return "SIGNUP";
            case Task::Type::SendMsg            : return "SEND_MSG";
            case Task::Type::SearchUser         : return "SEARCH_USER";
            case Task::Type::AddFriend          : return "ADD_FRIEND";
            case Task::Type::FriendRequestAccept: return "FRIEND_REQUEST_ACCEPT";
            case Task::Type::FriendRequestIgnore: return "FRIEND_REQUEST_IGNORE";
            default                             : return "UNKNOWN TASK";
        }
    }
}
