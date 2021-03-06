/*******************************************************************
 *                   Prattle/Server/Server.cpp                     *
 *    =========================================================    *
 *                                                                 *
 *    Contains implementation details of class Server described    *
 *    in Prattle/Server/Server.hpp.                                *
 *                                                                 *
 *    See https://github.com/TheIllusionistMirage/Prattle for      *
 *    more details.                                                *
 *                                                                 *
 *******************************************************************/


#include "../include/Server.hpp"
#include <regex>

namespace prattle
{
    Server::Server(): timeOut(sf::seconds(60)) ,
                      m_server_port{-1}        ,
                      m_configFile{SERVER_CONFIG_FILE ,
                                   std::ios_base::in}
    {
        // Parse the server configuration file.
        parseConfigFile();

        // Construct a blank friend request map.
        auto users = db.getAllUsernames();
        for (auto &itr : users)
            m_friendReqs.insert(std::make_pair(itr ,
                                                std::vector<std::string>{} ));

        // Port validity check.
        if (m_server_port == -1)
        {
            ERR_LOG("FATAL ERROR :: Server's open port unspecified! Please check \'" +
                    SERVER_CONFIG_FILE +
                    "\' for any erroneous values.");
            throw std::runtime_error("FATAL ERROR :: Server's open port unspecified! Please check \'" +
                                     SERVER_CONFIG_FILE +
                                     "\' for any erroneous values.");
        }

        // Bind listener to OPEN_PORT
        auto status = m_listener.listen(m_server_port);
        for(int i = 1; status != sf::Socket::Done && i <= 5 ; ++i)
        {
            sf::Time attempt_delay = sf::seconds(5);
            ERR_LOG("Unable to  bind the listener at port : " + to_string(m_server_port)
                + "\nRetrying in " + to_string(int(attempt_delay.asSeconds())) + " seconds. "
                + "\nAttempt : " + to_string(i));

            sf::sleep(attempt_delay);
            status = m_listener.listen(m_server_port);
        }
        if(status != sf::Socket::Done)
        {
            throw std::runtime_error("Unable to bind listener.");
        }

        DBG_LOG("sf::TcpListener object now listening to port " + to_string(m_server_port));

        m_selector.add(m_listener);
        DBG_LOG("sf::SocketSelector object now ready to interact with multiple sf::TcpSockets");

        m_running = true;
        DBG_LOG("Server went up at : " + getCurrentTimeAndDate());
    }

    void Server::parseConfigFile()
    {
        const std::regex field_pattern("(\\w+):([^:]+):"),
                            comment_pattern("\\s*#.*");
        enum data_type { INT, STRING };
        //Maps the field name in the config file to (data type, pointer to variable) of the field
        std::map<std::string, std::pair<data_type,void*>> fields_map;
        fields_map.insert({"open_port", {INT, &m_server_port}});
        fields_map.insert({"passphrase_hash", {STRING, &m_ctrlr_pass_hash}});
        if (m_configFile.is_open() && m_configFile.good())
        {
            m_configFile.seekg(std::ios_base::beg);
            std::string line;
            std::getline(m_configFile, line);

            for (unsigned int i = 1; !m_configFile.eof(); std::getline(m_configFile, line), i++)
            {
                std::string field;
                std::string value;
                std::smatch match;
                if(line.empty() || std::regex_match(line, comment_pattern))
                    continue;
                else if(std::regex_match(line, match, field_pattern))
                {
                    field = match[1].str();
                    value = match[2].str();
                }
                else
                {
                    WRN_LOG("Invalid field in config file :\n\t" + line);
                    continue;
                }

                auto mapping = fields_map.find(field);
                if(mapping == fields_map.end())
                {
                    WRN_LOG("Warning : Unrecognized field in config file, ignoring.");
                }
                else
                {
                    switch(mapping->second.first)
                    {
                    case INT:
                        *static_cast<int*>(mapping->second.second) = stoi(value);
                        break;
                    case STRING:
                        *static_cast<std::string*>(mapping->second.second) = value;
                        break;
                    default:
                        WRN_LOG("Unhandled data type");
                        break;
                    }
                }
            }
        }
        else
        {
            std::cerr << "Error reading config file" << std::endl;
            ERR_LOG("Error reading config file");
        }
    }

    void Server::run()
    {
        m_clock.restart();
        while (isRunning())
        {
            if (wait())
            {
                if (newConnectionRequest())
                {
                    addNewClient();
                }
                else
                {
                    receive();
                }
            }
            sf::sleep(sf::milliseconds(1));
        }
        m_listener.close();
        DBG_LOG("Server successfully shutdown at " + getCurrentTimeAndDate() + " .");
    }

    bool Server::isRunning()
    {
        return m_running;
    }

    bool Server::wait()
    {
        return m_selector.wait(timeOut);
    }

    bool Server::newConnectionRequest()
    {
        return m_selector.isReady(m_listener);
    }

    void Server::addNewClient()
    {
        std::unique_ptr<sf::TcpSocket> newClient{new sf::TcpSocket};
        if (m_listener.accept(*newClient) == sf::Socket::Done)
        {
            m_selector.add(*newClient);
            newClient->setBlocking(false);
            m_new_connections.push_back(std::move(newClient));
            DBG_LOG("New connection");
        }
    }

    bool Server::send(sf::Packet& packet, const std::string& username)
    {
        sf::Packet packetCopy{packet};
        auto itr = m_clients.find(username);
        bool result = true;
        if (itr == m_clients.end())
        {
            std::string reply;
            packet >> reply;
            std::cout << reply << std::endl;
            if(reply == SEND_MSG)
            {
                m_messages.insert(std::make_pair(username, packetCopy));
                DBG_LOG("Messesge for offline user " + username + "stored in server");
            }
            else
                WRN_LOG("Warning: Attempted to send packet to offline user \'" + username + "\'.");
        }
        else
        {
            if (itr->second->send(packetCopy) != sf::Socket::Done)
            {
                ERR_LOG("ERROR :: Error in sending packet to" + username);
                result = false;
            }
            else
                DBG_LOG("Messege for online user " + username + " sent");
        }
        return result;
    }

    bool Server::sendController(sf::Packet& packet)
    {
        if(!m_controller)
            return false;
        auto status = m_controller->send(packet);
        if(status != sf::Socket::Done)
        {
            ERR_LOG("Unable to send packet to controller. Status code : " + to_string(status));
        }
        return status == sf::Socket::Done;
    }

    void Server::receive()
    {
        for (auto itr = m_clients.begin(); itr != m_clients.end(); )
        {
            if (m_selector.isReady(*itr->second))
            {
                sf::Packet packet;
                auto status = (itr->second)->receive(packet);
                if (status == sf::Socket::Done)
                {
                    handleClientRequest(packet, itr->first);
                }
                else if (status == sf::Socket::Disconnected)
                {
                    DBG_LOG("[" + itr->first + "] left chat on " + getCurrentTimeAndDate());
                    m_selector.remove(*itr->second);
                    //itr = m_clients.erase(itr);

                    // Inform online friends of itr->first about his status
                    const auto& itr_friends = db.getFriends(itr->first);
                    std::map<std::string, std::vector<std::string>>::iterator jtr;
                    for (auto& friendName : itr_friends)
                    {
                        auto friend_itr = m_clients.find(friendName);
                        if (friend_itr != m_clients.end())
                        {
                            sf::Packet statusPacket;
                            statusPacket << STATUS_OFFLINE << itr->first;
                            if(send(statusPacket, friendName))
                            {
                                DBG_LOG("Notified \'" + friendName + "\' that \'" + itr->first + "\' logged out");
                            }
                            else
                                ERR_LOG("ERROR :: Error in sending status to \'" + friendName + "\' from the server");
                        }
                    }
                    itr = m_clients.erase(itr);
//                    jtr = m_friendReqs.find(itr->first);
//                    if (jtr != m_friendReqs.end())
//                        m_friendReqs.erase(jtr);
                    continue;
                }
            }
            itr++;
        }
        handleNewConnection();

        if(m_controller && m_selector.isReady(*m_controller))
        {
            receiveCommand();
        }
    }

    void Server::handleClientRequest(sf::Packet& packet, const std::string& sender)
    {
        std::string request;

        if (packet >> request)
        {
            DBG_LOG("New client request : " + request);

            if (request == SEND_MSG)
            {
                std::string rid, receiver, data;

                if (packet >> rid >> receiver >> data)
                {
                    sf::Packet rcvrReplyPacket;
                    rcvrReplyPacket << SEND_MSG << sender << data;

                    if (send(rcvrReplyPacket, receiver))
                    {
                        DBG_LOG("Sent message packet received from \'" + sender + "\' to \'" + receiver + "\'.");

                        sf::Packet replyPacket;
                        replyPacket << SEND_MSG_SUCCESS << rid;

                        if (send(replyPacket, sender))
                        {
                            DBG_LOG("Notified \'" + sender + "\' about message transmission success.");
                        }
                        else
                        {
                            ERR_LOG("Unable to notify \'" + sender + "\' about message transmission success.");
                        }
                    }
                    else
                    {
                        ERR_LOG("ERROR :: Unable to send \'" + receiver + "\' message received from \'" + sender + "\'.");

                        sf::Packet replyPacket;
                        replyPacket << SEND_MSG_FAILURE << rid;

                        if (send(replyPacket, sender))
                        {
                            DBG_LOG("Notified \'" + sender + "\' about message transmission failure.");
                        }
                        else
                        {
                            ERR_LOG("Unable to notify \'" + sender + "\' about message transmission failure.");
                        }
                    }
                }
                else
                {
                    ERR_LOG("ERROR :: Damaged packet received.");
                }
            }
            else if (request == SEARCH_USER)
            {
                std::string rid, query;

                if (packet >> rid >> query)
                {
                    sf::Packet searchResult;

                    auto matches = db.getMatchingUsers(sender, query);
                    if (matches.size() > 0)
                    {
                        searchResult << SEARCH_USER_RESULTS << rid << sf::Uint32(matches.size());

                        for (auto& i : matches)
                            searchResult << i;
                    }
                    else
                    {
                        DBG_LOG("No matches found!");
                        searchResult << SEARCH_USER_RESULTS << rid << 0;
                    }

                    if (!send(searchResult, sender))
                    {
                        ERR_LOG("ERROR :: Failed to send search results to \'" + sender + "\'.");
                    }
                    else
                        std::cout << "Sent results" << std::endl;
                }
                else
                {
                    ERR_LOG("ERROR while extracting data from packet.");
                }
            }

            else if (request == ADD_FRIEND)
            {
                std::string rid, user; // user denotes the receiver

                if (packet >> rid >> user)
                {
                    sf::Packet result;
                    std::string status;

                    /* Error checking code goes here, if any
                    */

                    status = "success";

                    /* //// */

                    result << ADD_FRIEND_REQ_ACK << rid << status << user;

                    /* Send acknowledgement to sender about his request */
                    if (send(result, sender))
                        DBG_LOG("Sent ack to \'" + sender + "\' about his friend request to \'" + user + "\'");
                    else
                        ERR_LOG("Unable to send ack to \'" + sender + "\' about his friend request to \'" + user + "\'");

                    /* pass on the friend request to 'sender's friend, 'user' */

                    auto f_itr = m_clients.find(user);
                    if (f_itr != m_clients.end()) // if friend is online, relay req immediately
                    {
                        result.clear();
                        result << ADD_FRIEND_REQ << sender;

                        if (send(result, user))
                            DBG_LOG("Forwarded \'" + sender + "\'s friend request to \'" + user + "\'");
                        else
                            ERR_LOG("Unable to forward \'" + sender + "\'s friend request to \'" + user + "\'");
                    }
                    //else
                    m_friendReqs[user].push_back(sender);
                }
                else
                    ERR_LOG("Corrupt packet received. Skipping operation \'" + ADD_FRIEND + "\'");
            }

            else if (request == ADD_FRIEND_ACCEPT)
            {
                std::string rid, user;

                // user now contains the name of the person who initiated the friend request
                if (packet >> rid >> user)
                {
                    sf::Packet result;
                    std::string status;

                    /* Error checking code goe here, if any */
                    status = "success";
                    /*/// */

                    //////

                    if (db.addNewFriend(sender, user))
                    {
                        result << ADD_FRIEND_REQ_ACK << rid << status << user;

                        // send the friend ack about his acceptance of the friend request
                        if (send(result, sender))
                            DBG_LOG("Sent ack to \'" + sender + "\' about his response to \'" + user + "\'s friend request");
                        else
                            ERR_LOG("Unable to send ack to \'" + sender + "\' about his response to \'" + user + "\'s friend request");

                        // notify the initiator of the friend request about the acceptance of the friend request
                        result.clear();
                        result << ADD_FRIEND_SUCCESS << sender;

                        if (send(result, user))
                            DBG_LOG("Notified \'" + user + "\' about \'" + sender + "\' accepting his friend request");
                        else
                            ERR_LOG("Unable to notify \'" + user + "\' about \'" + sender + "\' accepting his friend request");


                        auto friend_itr = m_clients.find(user);
                        if (friend_itr != m_clients.end()) //if the friend is online
                        {
                            sf::Packet statusPacket;
                            statusPacket << STATUS_ONLINE << user;
                            if(send(statusPacket, sender))
                            {
                                DBG_LOG("Notified \'" + sender + "\' that \'" + user + "\' is online");
                            }
                            else
                                ERR_LOG("ERROR :: Error in sending status to \'" + sender + "\' from the server");

                            statusPacket.clear();
                            statusPacket << STATUS_ONLINE << sender;
                            if(send(statusPacket, user))
                            {
                                DBG_LOG("Notified \'" + user + "\' that \'" + sender + "\' is online");
                            }
                            else
                                ERR_LOG("ERROR :: Error in sending status to \'" + user + "\' from the server");
                        }

                        // remove the corresponding friend request
                        //auto freqs = m_friendReqs[sender];
                        for (auto f = 0u; f < m_friendReqs[sender].size(); f++)
                            if (m_friendReqs[sender][f] == user)
                                m_friendReqs[sender].erase(m_friendReqs[sender].begin() + f);
                    }
                    else
                    {
                        ERR_LOG("Unable to add \'" + sender + "\' and \'" + user + "\' as friends of each other");
                    }
                }
                else
                    ERR_LOG("Corrupt packet received. Skipping operation \'" + ADD_FRIEND + "\'");
            }

            else if (request == ADD_FRIEND_IGNORE)
            {
                std::string rid, user;

                // user now contains the name of the person who initiated the friend request
                if (packet >> rid >> user)
                {
                    sf::Packet result;
                    std::string status;

                    /* Error checking code goe here, if any */
                    status = "success";
                    /*/// */

                    result << ADD_FRIEND_REQ_ACK << rid << status << sender;

                    // send the friend ack about his rejection of the friend request
                    if (send(result, sender))
                        DBG_LOG("Sent ack to \'" + sender + "\' about his response to \'" + user + "\'s friend request");
                    else
                        ERR_LOG("Unable to send ack to \'" + sender + "\' about his response to \'" + user + "\'s friend request");

                    /*

                    NOTE :
                    If person-1 sends a friend request to person-2, and person-2 rejects (ignores)
                    the request, then person-1 doesn't need to know about the rejection

                    */
                }
                else
                    ERR_LOG("Corrupt packet received. Skipping operation \'" + ADD_FRIEND + "\'");
            }
        }
        else
        {
            ERR_LOG("ERROR :: Damaged packet received.");
        }
    }

    void Server::handleNewConnection()
    {
        for(auto itr = m_new_connections.begin() ; itr != m_new_connections.end(); itr++)
        {
            if (m_selector.isReady(**itr))
            {
                sf::Packet packet;
                auto status = (*itr)->receive(packet);
                if (status == sf::Socket::Done)
                {
                    std::string request;
                    if (packet >> request)
                    {
                        if (request == LOGIN)
                        {
                            //DBG_LOG("Login attempt by a new connection.");
                            DBG_LOG("LOGIN requested received from a new connection");
                            std::string rid, sender, plainPassword;
                            if (packet >> rid >> sender >> plainPassword)
                            {
                                if (db.isValidPassword(sender, plainPassword))
                                {
                                    // send login success to client
                                    sf::Packet loginResult;
                                    loginResult << LOGIN_SUCCESS << rid << sf::Uint32(db.getFriends(sender).size());

                                    for (auto& friendName : db.getFriends(sender))
                                        loginResult << friendName;

                                    if ((*itr)->send(loginResult) != sf::Socket::Done)
                                    {
                                        ERR_LOG("ERROR :: Error in sending login acknowledgment to \'" + sender + "\' from the server");

                                        m_selector.remove(**itr);
                                        itr = m_new_connections.erase(itr);
                                        continue;
                                    }

                                    //std::cout << "[" + sender + "] joined chat on " << getCurrentTimeAndDate() << std::endl;
                                    DBG_LOG("[" + sender + "] joined chat on " + getCurrentTimeAndDate() + " .");

                                    // send to client all pending messages
                                    auto itr_end = m_messages.upper_bound(sender);
                                    for(auto itr_2 = m_messages.lower_bound(sender) ; itr_2 != itr_end ; ++itr_2)
                                    {
                                        if ((*itr)->send(itr_2->second) != sf::Socket::Done)
                                        {
                                            ERR_LOG("ERROR :: Error in sending packet from " + itr_2->first + " to " + sender + ".");
                                            m_selector.remove(**itr);
                                            itr = m_new_connections.erase(itr);
                                            continue;
                                        }
                                    }
                                    m_messages.erase(sender);
                                    m_clients.insert(std::make_pair(sender, std::move(*itr)));
                                    itr = m_new_connections.erase(itr);
                                    //m_friendReqs.insert(std::make_pair(sender, std::vector<std::string>{}));

                                    ///
                                    // notify all online friends of client aout his online status
                                    auto sender_friends = db.getFriends(sender);
                                    for (auto& friendName : sender_friends)
                                    {
                                        auto friend_itr = m_clients.find(friendName);
                                        if (friend_itr != m_clients.end()) //if friendName is online
                                        {
                                            //Notify `friendName` that `sender` came online
                                            sf::Packet statusPacket;
                                            statusPacket << STATUS_ONLINE << sender;
                                            if(send(statusPacket, friendName))
                                            {
                                                DBG_LOG("Notified \'" + friendName + "\' that \'" + sender + "\' logged in");
                                            }
                                            else
                                            {
                                                ERR_LOG("ERROR :: Error in sending status to \'" + friendName + "\' from the server");
                                                m_selector.remove(**itr);
                                                itr = m_new_connections.erase(itr);
                                                continue;
                                            }

                                            //Notify `sender` that friendName is online
                                            statusPacket.clear();
                                            statusPacket << STATUS_ONLINE << friendName;

                                            if(send(statusPacket, sender))
                                            {
                                                DBG_LOG("Notified \'" + sender + "\' that \'" + friendName + "\' is online");
                                            }
                                            else
                                            {
                                                ERR_LOG("ERROR :: Error in sending notification to \'" + sender + "\' from the server");
                                                m_selector.remove(**itr);
                                                itr = m_new_connections.erase(itr);
                                                continue;
                                            }
                                        }
                                    }

                                    // relay all pending friend requests

                                    if (m_friendReqs[sender].size() != 0)
                                    {
                                        for (auto&& fi : m_friendReqs[sender])
                                        {
                                            sf::Packet result;
                                            result << ADD_FRIEND_REQ << fi;
                                            DBG_LOG("friend req : " + fi);

                                            if (send(result, sender))
                                                DBG_LOG("Forwarded \'" + sender + "\'s friend request to \'" + fi + "\'");
                                            else
                                                ERR_LOG("Unable to forward \'" + sender + "\'s friend request to \'" + fi + "\'");
                                        }
                                    }
                                }
                                else
                                {
                                    sf::Packet loginResult;
                                    std::string details = "ERROR :: The username-password combination is not recognized.";
                                    loginResult << LOGIN_FAILURE << rid << sender << details;

                                    if (!((*itr)->send(loginResult)))
                                    {
                                        ERR_LOG("ERROR :: Unable to notify \'" + sender + "\' about invalid username-password combination they used to login.");
                                        m_selector.remove(**itr);
                                        itr = m_new_connections.erase(itr);
                                        continue;
                                    }

                                    DBG_LOG("Login attempt with invalid password/username.\nConnection closed.");

                                    m_selector.remove(**itr);
                                    itr = m_new_connections.erase(itr);
                                }
                            }
                            else
                            {
                                ERR_LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                                m_selector.remove(**itr);
                                itr = m_new_connections.erase(itr);
                            }
                        }
                        else if (request == SIGNUP)
                        {
                            DBG_LOG("SIGNUP requested received from a new connection");
                            std::string rid, sender, plainPassword;
                            if (packet >> rid >> sender >> plainPassword)
                            {
                                if (db.addNewUser(sender, plainPassword))
                                {
                                    m_friendReqs.insert(std::make_pair(sender, std::vector<std::string>{}));
                                    sf::Packet signupResult;
                                    signupResult << SIGNUP_SUCCESS << rid;

                                    if ((*itr)->send(signupResult) == sf::Socket::Done)
                                    {
                                        DBG_LOG("Notified '" + sender + "\' about signup success.");
                                    }
                                    else
                                    {
                                        ERR_LOG("ERROR :: Could not send signup acknowledgment to \'" + sender + "\'.");
                                        m_selector.remove(**itr);
                                        itr = m_new_connections.erase(itr);
                                        continue;
                                    }

                                    m_selector.remove(**itr);
                                    itr = m_new_connections.erase(itr);
                                }
                                else
                                {
                                    sf::Packet signupResult;
                                    std::string details = "ERROR :: The username \'" + sender + "\' exists.";
                                    signupResult << SIGNUP_FAILURE << rid << details;
                                    if (!send(signupResult, sender))
                                    {
                                        ERR_LOG("ERROR :: Unable to notify \'" + sender + "\' about unsuccessful attempt to sign up");
                                        m_selector.remove(**itr);
                                        itr = m_new_connections.erase(itr);
                                        continue;
                                    }
                                }
                            }
                            else
                            {
                                ERR_LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                            }
                        }
                        else if(request == S_CONTROLLER_ATTACH)
                        {
                            sf::Packet reply;
                            if(m_controller)
                            {
                                reply << "A controller is already attached. Skipping.";
                                m_selector.remove(**itr);
                            }
                            else
                            {
                                std::string passphrase;
                                packet >> passphrase;
                                if(sha256(passphrase) == m_ctrlr_pass_hash)
                                {
                                    reply << "ack";
                                    m_controller = std::move(*itr);
                                    sendController(reply);
                                    DBG_LOG("Controller attached.\nController IP : " + m_controller->getRemoteAddress().toString());
                                }
                                else
                                {
                                    reply << "Invalid Passphrase!";
                                    WRN_LOG("Wrong passphrase given by controller!");
                                    m_controller = std::move(*itr);
                                    sendController(reply);
                                    m_selector.remove(*m_controller);
                                    m_controller.reset();
                                }
                            }
                            itr = m_new_connections.erase(itr);
                        }
                        else
                        {
                            ERR_LOG("ERROR :: An unknown request \'" + request + "\' is being tried be executed by the server.");
                        }
                    }
                    else
                    {
                        ERR_LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                    }
                }
                else if (status == sf::Socket::Disconnected ||
                          status == sf::Socket::Error)
                {
                    DBG_LOG("New connection disconnected");
                    m_selector.remove(**itr);
                    itr = m_new_connections.erase(itr);
                }

                else if (status == sf::Socket::NotReady ||
                       status == sf::Socket::Partial)
                {

                    DBG_LOG("New connection disconnected");
                    sf::Packet errPacket;
                    std::string msg = "[Prattle Server] :: Unable to handle request.\n";
                    errPacket << msg;
                    if ((*itr)->send(errPacket) != sf::Socket::Done)
                    {
                        ERR_LOG("ERROR :: Error in sending error message to the remote client from the server");
                        m_selector.remove(**itr);
                        itr = m_new_connections.erase(itr);
                        continue;
                    }

                    m_selector.remove(**itr);
                    itr = m_new_connections.erase(itr);
//                    if (itr != m_new_connections.end())
//                        std::cout << "itr != nullptr" << std::endl;
//                    else
//                        std::cout << "Size of new conn. : " << m_new_connections.size() << std::endl;
//                    itr++;
                }
            }
        }
    }

    void Server::receiveCommand()
    {
        sf::Packet packet;
        auto status = m_controller->receive(packet);
        if(status == sf::Socket::Done)
        {
            std::string request;
            packet >> request;
            sf::Packet reply;
            packet << request;
            if(request == S_CONTROLLER_SHUTDOWN)
            {
                reply << "ack";
                shutdown();
            }
            else if(request == S_CONTROLLER_SHOW_USERS)
            {
                std::string replyStr;
                for(auto& i : m_clients)
                {
                    replyStr += i.first + '\n';
                }
                if (replyStr.size())
                    replyStr.erase(replyStr.size()-1); //remove the last newline
//                else
//                    replyStr = "None.";
                reply << "ack" << replyStr;
            }
            else if(request == S_CONTROLLER_STATS)
            {
                std::string replyStr;
                replyStr += "Uptime : " + to_string(m_clock.getElapsedTime().asSeconds()/60.f) + " minutes\n";
                replyStr += "Users logged : " + to_string(m_clients.size()) + '\n';
                replyStr += "New Connections pending : " + to_string(m_new_connections.size());
                reply << "ack" << replyStr;
            }
            else if(request == S_CONTROLLER_REMOVE_USER)
            {
                std::string user;
                packet >> user;
                if(db.removeUser(user))
                {
                    reply << "ack";
                    DBG_LOG("User " + user + " removed from the database.");
                }
                else
                {
                    reply << "Removing user failed.";
                }
            }
            else
            {
                reply << "Unrecognized command! (see help for usage)";
            }
            sendController(reply);
        }
        else if(status == sf::Socket::Disconnected)
        {
            DBG_LOG("Controller detached.");
            m_selector.remove(*m_controller);
            m_controller.reset(nullptr);
        }
        else if(status == sf::Socket::Error)
        {
            ERR_LOG("Unable to receive packet from controller. Status code : " + to_string(status));
        }
    }
    // The shutdown() function shouldn't do anything
    // nontrivial or any non-threadsafe stuff because
    // it is called by the signal handler (in a
    // different thread)
    void Server::shutdown()
    {
        m_running = false;
    }
}
