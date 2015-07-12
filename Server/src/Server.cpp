/**

    Prattle/Server/Server.cpp
    ===============================

    Contains implementations of class Server declared in Prattle/Server/Server.hpp

*/

#include "../include/Server.hpp"
#include <regex>

namespace prattle
{
    Server::Server(): timeOut(sf::seconds(60))
                    , m_server_port{-1}
                    , m_configFile{SERVER_CONFIG_FILE, std::ios_base::in}
                    //, m_globalChat{""}
    {
        parseConfigFile();

        if (m_server_port == -1)
        {
            ERR_LOG("FATAL ERROR :: Server's open port unspecified! Please check \'" + SERVER_CONFIG_FILE + "\' for any erroneous values.");
            throw std::runtime_error("FATAL ERROR :: Server's open port unspecified! Please check \'" + SERVER_CONFIG_FILE + "\' for any erroneous values.");
        }

        auto status = m_listener.listen(m_server_port);
        for(int i = 1; status != sf::Socket::Done && i <= 5 ; ++i)
        {
            sf::Time attempt_delay = sf::seconds(5);
            ERR_LOG("Unable to  bind the listener at port : " + std::to_string(m_server_port)
                + "\n Retrying in " + std::to_string(int(attempt_delay.asSeconds())) + " seconds. "
                + "\n Attempt : " + std::to_string(i));
            sf::sleep(attempt_delay);
            status = m_listener.listen(m_server_port);
        }
        if(status != sf::Socket::Done)
        {
            throw std::runtime_error("Unable to bind listener.");
        }
        DBG_LOG("sf::TcpListener object now listening to port " + std::to_string(m_server_port));

        m_selector.add(m_listener);
        DBG_LOG("sf::SocketSelector object now ready to interact with multiple sf::TcpSockets");

        m_running = true;
        DBG_LOG("Server went up at : " + getCurrentTimeAndDate());
    }

    void Server::parseConfigFile()
    {
        static std::regex field_pattern("(\\w+):([^:]+):"),
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
                    WRN_LOG("Warning : Unrecognized field in conifg file, ignoring.");
                }
                else
                {
                    switch(mapping->second.first)
                    {
                    case INT:
                        *static_cast<int*>(mapping->second.second) = std::stoi(value);
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
        shutdown();
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
        }
    }

    bool Server::send(const sf::Packet& packet, const std::string& username)
    {
        sf::Packet packet_;
        auto itr = m_clients.find(username);
        bool result = true;
        if (itr == m_clients.end())
        {
            m_messages.insert(std::make_pair(username, packet));
            DBG_LOG("Messesge for offline user " + username + "stored in server");
        }
        else
        {
            std::string reply;
            packet_ >> reply;
            if(reply == SEND_MSG)
            {
                DBG_LOG("Messege for online user " + username + " sent");
                if (itr->second->send(packet_) != sf::Socket::Done)
                {
                    ERR_LOG("ERROR :: Error in sending packet to" + username);
                    result = false;
                }
            }
            else
                WRN_LOG("Warning: Attempted to send packet to offline user.");
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
            ERR_LOG("Unable to send packet to controller. Status code : " + std::to_string(status));
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
            if (request == SEND_MSG)
            {
                std::string receiver, data;
                if (packet >> receiver >> data)
                {
                    sf::Packet newPacket;
                    newPacket << SEND_MSG << sender << data;
                    if (!send(newPacket, receiver))
                    {
                        ERR_LOG("ERROR :: Packet from \'" + sender + "\' was not sent to \'" + receiver + "\'.");

                        sf::Packet replyPacket;
                        std::string details = "ERROR :: Packet from \'" + sender + "\' was not sent to \'" + receiver + "\'.";
                        replyPacket << SEND_MSG_FAILURE << djb2_hash(data) << details;
                        if (!send(replyPacket, sender))
                        {
                            ERR_LOG("ERROR :: Error in notifying \'" + sender + "\' about message transmission's failure.");
                        }
                    }
                    else
                    {
                        sf::Packet replyPacket;
                        replyPacket << SEND_MSG_SUCCESS << djb2_hash(data);
                        if (!send(replyPacket, sender))
                        {
                            ERR_LOG("ERROR :: Error in notifying \'" + sender + "\' about message transmission's success.");
                        }
                        else
                        {
                            DBG_LOG("Notified \'" + sender + "\' about message transmission's success.");
                        }
                    }
                }
                else
                {
                    ERR_LOG("ERROR while extracting data from packet.");
                }
            }
            else if (request == SEARCH_USER)
            {
                std::string query;
                if (packet >> query)
                {
                    //searchDatabase(query);
                    // This part is a WIP. For now only the exact match for query
                    // gets sent to sender. Later on matching results feature will get added.
                    if (db.isUserRegistered(query))
                    {
                        sf::Packet searchResult;
                        searchResult << SEARCH_USER_RESULTS << 1 << query;
                        if (!send(searchResult, sender))
                        {
                            ERR_LOG("ERROR :: Failed to send search results to \'" + sender + "\'.");
                        }
                    }
                    else
                    {
                        sf::Packet searchResult;
                        searchResult << SEARCH_USER_RESULTS << 0;
                        if (!send(searchResult, sender))
                        {
                            ERR_LOG("ERROR :: Failed to send search results to \'" + sender + "\'.");
                        }
                    }
                }
                else
                {
                    ERR_LOG("ERROR while extracting data from packet.");
                }
            }
            else if (request == ADD_FRIEND)
            {
                std::string user;
                if (db.isUserRegistered(user))
                {
                    db.addNewFriend(sender, user);
                    sf::Packet result;
                    result << ADD_FRIEND_SUCCESS;
                    if (send(result, sender))
                    {
                        DBG_LOG("Sent success info to \'" + sender + "\' for successfully adding \'" + user + "\' as a friend.");
                    }
                    else
                        ERR_LOG("ERROR :: Failed to send success info to \'" + sender + "\' for successfully adding \'" + user + "\' as a friend.");
                    result.clear();

                    /*result << ADD_FRIEND_SUCCESS << SERVER << user << sender;
                    if (send(result))
                    {
                        LOG("Sent success info to \'" + user + "\' for successfully adding \'" + sender + "\' as a friend.");
                    }

                    else
                        ERR_LOG("ERROR :: Error in sending friend add acknowledgement to \'" + user + "\'");*/
                    //result.clear();

                    sf::Packet statusPacket;
                    auto friend_itr = m_clients.find(user);
                    if (friend_itr != m_clients.end())
                    {
                        result << ADD_FRIEND_SUCCESS << sender;
                        if (send(result, user))
                        {
                            DBG_LOG("Sent success info to \'" + user + "\' for successfully adding \'" + sender + "\' as a friend.");
                        }
                        else
                            ERR_LOG("ERROR :: Error in sending friend add acknowledgement to \'" + user + "\'");

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
                }
                else
                {
                    sf::Packet result;
                    std::string details = "ERROR :: \'" + user + "\' is not a registered member.";
                    result << ADD_FRIEND_FAILURE << details;
                    if (!send(result, sender))
                    {
                        ERR_LOG("ERROR :: Failed to send failure info to \'" + sender + "\' about an unsuccessful attempt to \'" + user + "\' as a friend.");
                    }
                }
            }
            else
            {
                ERR_LOG("ERROR :: Unknown request \'" + request + "\' sent to the server.");
            }
        }
        else
        {
            ERR_LOG("ERROR :: Damaged packet received.");
        }
    }

    void Server::handleNewConnection()
    {
        for(auto itr = m_new_connections.begin() ; itr != m_new_connections.end(); )//i++)
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
                            std::string sender, plainPassword;
                            if (packet >> sender >> plainPassword)
                            {
                                if (db.isValidPassword(sender, plainPassword))
                                {
                                    sf::Packet loginResult;
                                    loginResult << LOGIN_SUCCESS << SERVER << sender << sf::Uint32(db.getFriends(sender).size());

                                    for (auto& friendName : db.getFriends(sender))
                                        loginResult << friendName;

                                    if ((*itr)->send(loginResult) != sf::Socket::Done)
                                    {
                                        ERR_LOG("ERROR :: Error in sending login acknowledgment to \'" + sender + "\' from the server");

                                        m_selector.remove(**itr);
                                        itr = m_new_connections.erase(itr);
                                        continue;
                                    }

                                    std::cout << "o [" + sender + "] joined chat on " << getCurrentTimeAndDate() << std::endl;
                                    DBG_LOG("[" + sender + "] joined chat on " + getCurrentTimeAndDate() + " .");
                                    auto itr_end = m_messages.upper_bound(sender);
                                    for(auto itr_2 = m_messages.lower_bound(sender) ; itr_2 != itr_end ; ++itr_2)
                                    {
                                        if ((*itr)->send(itr_2->second) != sf::Socket::Done)
                                        {
                                            ERR_LOG("ERROR :: Error in sending packet from " + itr_2->first + " to " + sender + ".");
                                        }
                                    }
                                    m_messages.erase(sender);
                                    m_clients.insert(std::make_pair(sender, std::move(*itr)));
                                    itr = m_new_connections.erase(itr);

                                    auto sender_friends = db.getFriends(sender);
                                    for (auto& friendName : sender_friends)
                                    {
                                        auto friend_itr = m_clients.find(friendName);
                                        if (friend_itr != m_clients.end()) //if friendName is online
                                        {
                                            //Notify `friendName` that `sender` came online
                                            sf::Packet statusPacket;
                                            statusPacket << sender;
                                            if(send(statusPacket, friendName))
                                            {
                                                DBG_LOG("Notified \'" + friendName + "\' that \'" + sender + "\' logged in");
                                            }
                                            else
                                                ERR_LOG("ERROR :: Error in sending status to \'" + friendName + "\' from the server");
                                            //Notify `sender` that friendName is online
                                            statusPacket.clear();
                                            statusPacket << STATUS_ONLINE << friendName;
                                            if(send(statusPacket, sender))
                                            {
                                                DBG_LOG("Notified \'" + sender + "\' that \'" + friendName + "\' is online");
                                            }
                                            else
                                                ERR_LOG("ERROR :: Error in sending notification to \'" + sender + "\' from the server");
                                        }
                                    }
                                }
                                else
                                {
                                    sf::Packet loginResult;
                                    std::string details = "ERROR :: The username-password combination is not recognized.";
                                    loginResult << LOGIN_FAILURE << SERVER << sender << details;
                                    if (!(*itr)->send(loginResult))
                                    {
                                        ERR_LOG("ERROR :: Unable to notify \'" + sender + "\' about invalid username-password combination they used to login.");
                                    }
                                }
                            }
                            else
                            {
                                ERR_LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                            }
                        }
                        else if (request == SIGNUP)
                        {
                            std::string sender, plainPassword;
                            if (packet >> sender >> plainPassword)
                            {
                                if (db.addNewUser(sender, plainPassword))
                                {
                                    sf::Packet signupResult;
                                    signupResult << SIGNUP_SUCCESS << SERVER << sender;

                                    if ((*itr)->send(signupResult) != sf::Socket::Done)
                                    {
                                        ERR_LOG("ERROR :: Could not send signup acknowledgment to \'" + sender + "\'.");
                                    }
                                    m_selector.remove(**itr);
                                    itr = m_new_connections.erase(itr);
                                }
                                else
                                {
                                    sf::Packet signupResult;
                                    std::string details = "ERROR :: The username \'" + sender + "\' exists.";
                                    signupResult << SIGNUP_FAILURE << details;
                                    if (!send(signupResult, sender))
                                    {
                                        ERR_LOG("ERROR :: Unable to notify \'" + sender + "\' about unsuccessful attempt to sign up");
                                    }
                                }
                            }
                            else
                            {
                                ERR_LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                            }
                        }
                        else if(request == "controller_attach")
                        {
                            sf::Packet reply;
                            if(m_controller)
                            {
                                reply << "A controller is already attached.";
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
                                    DBG_LOG("Controller attached.\n Controller IP : " + m_controller->getRemoteAddress().toString());
                                }
                                else
                                {
                                    reply << "Invalid Passphrase !";
                                    WRN_LOG("Wrong passphrase given by controller !");
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
                else if (status == sf::Socket::Disconnected || status == sf::Socket::Error)
                {
                    m_selector.remove(**itr);
                    itr = m_new_connections.erase(itr);
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
            if(request == "shutdown")
            {
                reply << "ack";
                shutdown();
            }
            else if(request == "show_logged_users")
            {
                std::string replyStr;
                for(auto& i : m_clients)
                {
                    replyStr += i.first + '\n';
                }
                replyStr.resize(replyStr.size()-1); //remove the last newline
                reply << "ack" << replyStr;
            }
            else if(request == "print_stats")
            {
                std::string replyStr;
                replyStr += "Uptime : " + std::to_string(m_clock.getElapsedTime().asSeconds()/60.f) + " minutes\n";
                replyStr += "Users logged : " + std::to_string(m_clients.size()) + '\n';
                replyStr += "New Connections pending : " + std::to_string(m_new_connections.size());
                reply << "ack" << replyStr;
            }
            else if(request == "remove_user")
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
                reply << "That command is unavailable.";
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
            ERR_LOG("Unable to receive packet from controller. Status code : " + std::to_string(status));
        }
    }
    void Server::shutdown()
    {
        m_listener.close();
        m_running = false;
        DBG_LOG("Server successfully shutdown at " + getCurrentTimeAndDate() + " .");
    }
}
