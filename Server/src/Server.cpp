/**

    Prattle/Server/Server.cpp
    ===============================

    Contains implementations of class Server declared in Prattle/Server/Server.hpp

*/

#include "../include/Server.hpp"

namespace prattle
{
    Server::Server(): timeOut(sf::seconds(60))
                    , m_server_port{-1}
                    , m_configFile{SERVER_CONFIG_FILE, std::ios::in}
                    //, m_globalChat{""}
    {
        if (m_configFile.is_open() && m_configFile.good())
        {
            parseConfigFile();
        }

        if (m_server_port == -1)
        {
            LOG("FATAL ERROR :: Server's open port unspecified! Please check \'" + SERVER_CONFIG_FILE + "\' for any erroneous values.");
            throw std::runtime_error("FATAL ERROR :: Server's open port unspecified! Please check \'" + SERVER_CONFIG_FILE + "\' for any erroneous values.");
        }

        auto status = m_listener.listen(m_server_port);
        LOG("sf::TcpListener object now listening to port " + std::to_string(m_server_port));

        if(status != sf::Socket::Done)
        {
            LOG("FATAL ERROR : Error binding the listener at " + std::to_string(m_server_port));
            throw std::runtime_error("Fatal error : Error binding the listener at " + std::to_string(m_server_port));
        }

        m_selector.add(m_listener);
        LOG("sf::SocketSelector object now ready to interact with multiple sf::TcpSockets");

        m_running = true;
        LOG("Server now up & running");

        std::cout << "=============================" << std::endl;
        std::cout << "|      Prattle - v 0.1      |" << std::endl;
        std::cout << "|     ( Always be near )    |" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << "|                           |" << std::endl;
        std::cout << "|          SERVER           |" << std::endl;
        std::cout << "=============================" << std::endl << std::endl;

        std::cout << "By team Prattle" << std::endl << std::endl;

        std::cout << "--- Server went up at " << getCurrentTimeAndDate() << " ---" << std::endl;
        std::cout << "--- Listening to incoming connections at port " << m_server_port << " ---" << std::endl << std::endl;
        std::cout << "--- SERVER LOG ---" << std::endl << std::endl;

        LOG("Server went up at : " + getCurrentTimeAndDate());
    }

    void Server::parseConfigFile()
    {
        if (m_configFile.is_open() && m_configFile.good())
        {
            m_configFile.seekg(std::ios::beg);
            std::string line;
            std::getline(m_configFile, line);

            for (unsigned int i = 1; !m_configFile.eof(); std::getline(m_configFile, line), i++)
            {
                std::string field;
                std::string value;

                if(line[0] == '#' || line.size() < 2)
                    continue;

                auto first_colon = line.find(':', 0);
                auto second_colon = line.find(':', first_colon + 1);

                if(first_colon == std::string::npos
                   || second_colon == std::string::npos)
                {
                    LOG("WARNING :: Invalid configuration value at " + SERVER_CONFIG_FILE + " : " + std::to_string(i));
                    continue;
                }

                field = line.substr(0, first_colon);
                value = line.substr(first_colon + 1, second_colon - first_colon - 1);

                if (field == "OPEN_PORT")
                    m_server_port = std::stoi(value);
            }
        }
    }

    void Server::run()
    {
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
            newConnections.push_back(std::move(newClient));
        }
    }

    bool Server::send(const sf::Packet& packet)
    {
        sf::Packet packetCopy{packet};

        std::string protocol;

        if (packetCopy >> protocol)
        {
            if ( protocol == LOGIN_SUCCESS       ||
                  protocol == LOGIN_FAILURE       ||
                   protocol == SIGNUP_SUCCESS      ||
                    protocol == SIGNUP_FAILURE      ||
                     protocol == SEND_MSG            ||
                      protocol == SEND_MSG_SUCCESS    ||
                       protocol == SEND_MSG_FAILURE    ||
                        protocol == SEARCH_USER_RESULTS ||
                         protocol == ADD_FRIEND_SUCCESS  ||
                          protocol == ADD_FRIEND_FAILURE  ||
                           protocol == NOTIF_LOGIN         ||
                            protocol == NOTIF_LOGOUT        ||
                             protocol == NOTIF_ONLINE )
            {
                std::string sender, username;

                if (packetCopy >> sender >> username)       // NOTE : the sender in this case is ALWAYS the server.
                {
                    auto itr = m_clients.find(username);
                    bool result = true;

                    if (itr == m_clients.end())
                    {
                        // NOTE : the sender in this case is ALWAYS the server.
                        m_messages.insert(std::make_pair(username, std::make_pair(sender, packetCopy)));
                        LOG("Messesge for offline user " + username + "Stored in server");
                    }

                    else
                    {
                        LOG("Messege for online user " + username + " sent");
                        if (itr->second->send(packetCopy) != sf::Socket::Done)
                        {
                            LOG("ERROR :: Error in sending packet from " + sender + " to " + username + ".");
                            result = false;
                        }
                    }

                    return result;
                }

                else
                {
                    LOG("ERROR :: A damaged packet is being tried to be sent by the server.");
                    return false;
                }
            }

            else
            {
                LOG("ERROR :: An unknown protocol \'" + protocol + "\' is being tried to be executed by the server.");
                return false;
            }
        }

        else
        {
            LOG("ERROR :: A damaged packet is being tried to be sent by the server.");
            return false;
        }
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
                    std::string protocol;

                    if (packet >> protocol)
                    {
                        if (protocol == SEND_MSG)
                        {
                            std::string sender, receiver, data;

                            if (packet >> sender >> receiver >> data)
                            {
                                /*if (receiver == "Open Chat")
                                {
                                    m_globalChat += sender + " : " + data + "\n";

                                    sf::Packet pckt;
                                    pckt << SEND_MSG << SERVER << sender << SERVER << m_globalChat;

                                    if (!send(pckt))
                                    {
                                        LOG("ERROR :: Packet from \'" + SERVER + "\' was not sent to \'" + sender + "\'.");
                                    }

                                    continue;
                                }*/

                                sf::Packet newPacket;
                                newPacket << SEND_MSG << SERVER << receiver << sender << data;

                                if (!send(newPacket))
                                {
                                    LOG("ERROR :: Packet from \'" + sender + "\' was not sent to \'" + receiver + "\'.");

                                    sf::Packet replyPacket;
                                    std::string details = "ERROR :: Packet from \'" + sender + "\' was not sent to \'" + receiver + "\'.";
                                    replyPacket << SEND_MSG_FAILURE << SERVER << sender << details;

                                    if (!send(replyPacket))
                                    {
                                        LOG("ERROR :: Error in notifying \'" + sender + "\' about message transmission's failure.");
                                    }
                                }

                                else
                                {
                                    sf::Packet replyPacket;
                                    replyPacket << SEND_MSG_SUCCESS << SERVER << sender;

                                    if (!send(replyPacket))
                                    {
                                        LOG("ERROR :: Error in notifying \'" + sender + "\' about message transmission's success.");
                                    }
                                    else
                                    {
                                        LOG("Notified \'" + sender + "\' about message transmission's success.");
                                    }
                                }
                            }

                            else
                            {
                                LOG("ERROR :: A damaged packet was received by the server from \'" + itr->first + "\'.");
                            }
                        }

                        else if (protocol == SEARCH_USER)
                        {
                            std::string sender, receiver, query;

                            if (packet >> sender >> receiver >> query)
                            {
                                if (receiver == SERVER)
                                {
                                    //searchDatabase(query);
                                    // This part is a WIP. For now only the exact match for query
                                    // gets sent to sender. Later on matching results feature will get added.

                                    if (db.isUserRegistered(query))
                                    {
                                        sf::Packet searchResult;
                                        searchResult << SEARCH_USER_RESULTS << SERVER << sender << query;

                                        if (!send(searchResult))
                                        {
                                            LOG("ERROR :: Failed to send search results to \'" + sender + "\'.");
                                        }
                                    }

                                    else
                                    {
                                        sf::Packet searchResult;
                                        searchResult << SEARCH_USER_RESULTS << SERVER << sender << "";

                                        if (!send(searchResult))
                                        {
                                            LOG("ERROR :: Failed to send search results to \'" + sender + "\'.");
                                        }
                                    }
                                }

                                else
                                {
                                    LOG("ERROR :: A damaged packet was received by the server from \'" + itr->first + "\'.");
                                }
                            }

                            else
                            {
                                LOG("ERROR :: A damaged packet was received by the server from \'" + itr->first + "\'.");
                            }
                        }

                        else if (protocol == ADD_FRIEND)
                        {
                            std::string sender, receiver, user;

                            if (packet >> sender >> receiver >> user)
                            {
                                if (receiver == SERVER)
                                {
                                    if (db.isUserRegistered(user))
                                    {
                                        db.addNewFriend(sender, user);

                                        sf::Packet result;
                                        result << ADD_FRIEND_SUCCESS << SERVER << sender;

                                        if (!send(result))
                                        {
                                            LOG("ERROR :: Failed to send success info to \'" + sender + "\' for successfully adding \'" + user + "\' as a friend.");
                                        }

                                        result.clear();

                                        result << ADD_FRIEND_SUCCESS << SERVER << user << sender;
                                        if (!send(result))
                                        {
                                            LOG("ERROR :: Failed to send success info to \'" + user + "\' for successfully adding \'" + sender + "\' as a friend.");
                                        }
                                    }

                                    else
                                    {
                                        sf::Packet result;
                                        std::string details = "ERROR :: \'" + user + "\' is not a registered member.";
                                        result << ADD_FRIEND_FAILURE << SERVER << sender << details;

                                        if (!send(result))
                                        {
                                            LOG("ERROR :: Failed to send failure info to \'" + sender + "\' about an unsuccessful attempt to \'" + user + "\' as a friend.");
                                        }
                                    }
                                }
                            }

                            else
                            {
                                LOG("ERROR :: A damaged packet was received by the server from \'" + itr->first + "\'.");
                            }
                        }

                        /*else if (protocol == NOTIF_ONLINE)
                        {
                            std::string sender, receiver;

                            if (packet >> sender >> receiver)
                            {
                                Record itr_record = db.getRecord(itr->first);
                                auto itr_friends = itr_record.friends;

                                for (auto& friendName : itr_friends)
                                {
                                    auto friend_itr = m_clients.find(friendName);

                                    if (friend_itr != m_clients.end())
                                    {
                                        //std::cout << friendName << " is online" << std::endl;
                                        sf::Packet notifPacket;
                                        notifPacket << NOTIF_ONLINE << SERVER << itr->first;

                                        if(send(notifPacket))
                                        {
                                            LOG("Notified \'" + itr->first + "\' that \'" + friendName + "\' is online");
                                        }

                                        else
                                            LOG("ERROR :: Error in sending notification to \'" + friendName + "\' from the server");
                                    }
                                }
                            }

                            else
                            {
                                LOG("ERROR :: Damaged packet received by server");
                            }
                        }*/

                        else
                        {
                            LOG("ERROR :: An unknown protocol \'" + protocol + "\' is being tried be executed by the server.");
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet was received by the server from \'" + itr->first + "\'.");
                    }
                }

                else if (status == sf::Socket::Disconnected)
                {
                    // TODO : add the code to make the client's status offline here

                    std::cout << " x [" + itr->first + "] left chat " << getCurrentTimeAndDate() << std::endl;
                    LOG("[" + itr->first + "] left chat on " + getCurrentTimeAndDate());
                    m_selector.remove(*itr->second);
                    //itr = m_clients.erase(itr);

                    // Send a notif to all online friends of itr->first about his status

                    auto itr_friends = db.getFriends(itr->first);

                    for (auto& friendName : itr_friends)
                    {
                        auto friend_itr = m_clients.find(friendName);

                        if (friend_itr != m_clients.end())
                        {
                            //std::cout << friendName << " is online" << std::endl;
                            sf::Packet notifPacket;
                            notifPacket << NOTIF_LOGOUT << SERVER << friendName << itr->first;

                            if(send(notifPacket))
                            {
                                LOG("Notified \'" + friendName + "\' that \'" + itr->first + "\' logged out");
                            }

                            else
                                LOG("ERROR :: Error in sending notification to \'" + friendName + "\' from the server");
                        }
                    }

                    //
                    itr = m_clients.erase(itr);

                    continue;
                }
            }

            itr++;
        }

        for(auto itr = newConnections.begin() ; itr != newConnections.end(); )//i++)
        {
            if (m_selector.isReady(**itr))
            {
                sf::Packet packet;

                auto status = (*itr)->receive(packet);

                if (status == sf::Socket::Done)
                {
                    std::string protocol;

                    if (packet >> protocol)
                    {
                        if (protocol == LOGIN)
                        {
                            std::string sender, receiver, plainPassword;

                            if (packet >> sender >> receiver >> plainPassword)
                            {
                                if (receiver == SERVER)
                                {
                                    if (db.isValidPassword(sender, plainPassword))
                                    {
                                        sf::Packet loginResult;
                                        loginResult << LOGIN_SUCCESS << SERVER << sender << sf::Uint32(db.getFriends(sender).size());

                                        for (auto& friendName : db.getFriends(sender))
                                            loginResult << friendName;

                                        if ((*itr)->send(loginResult) != sf::Socket::Done)
                                        {
                                            LOG("ERROR :: Error in sending login acknowledgment to \'" + sender + "\' from the server");

                                            m_selector.remove(**itr);
                                            itr = newConnections.erase(itr);
                                            continue;
                                        }

                                        std::cout << "o [" + sender + "] joined chat on " << getCurrentTimeAndDate() << std::endl;
                                        LOG("[" + sender + "] joined chat on " + getCurrentTimeAndDate() + " .");

                                        auto itr_end = m_messages.upper_bound(sender);

                                        for(auto itr_2 = m_messages.lower_bound(sender) ; itr_2 != itr_end ; ++itr_2)
                                        {
                                            if ((*itr)->send(itr_2->second.second) != sf::Socket::Done)
                                            {
                                                LOG("ERROR :: Error in sending packet from " + itr_2->first + " to " + sender + ".");
                                            }
                                        }

                                        m_messages.erase(sender);
                                        m_clients.insert(std::make_pair(sender, std::move(*itr)));
                                        itr = newConnections.erase(itr);

                                        //

                                        auto sender_friends = db.getFriends(sender);
                                        sf::Packet onlinePacket;

                                        for (auto& friendName : sender_friends)
                                        {
                                            auto friend_itr = m_clients.find(friendName);

                                            if (friend_itr != m_clients.end())
                                            {
                                                //std::cout << friendName << " is online" << std::endl;
                                                sf::Packet notifPacket;
                                                notifPacket << NOTIF_LOGIN << SERVER << friendName << sender;

                                                if(send(notifPacket))
                                                {
                                                    LOG("Notified \'" + friendName + "\' that \'" + sender + "\' logged in");
                                                }

                                                else
                                                    LOG("ERROR :: Error in sending notification to \'" + friendName + "\' from the server");

                                                notifPacket.clear();
                                                notifPacket << NOTIF_ONLINE << SERVER << sender << friendName;

                                                if(send(notifPacket))
                                                {
                                                    LOG("Notified \'" + sender + "\' that \'" + friendName + "\' is online");
                                                }

                                                else
                                                    LOG("ERROR :: Error in sending notification to \'" + sender + "\' from the server");
                                            }
                                        }

                                        //
                                    }

                                    else
                                    {
                                        sf::Packet loginResult;
                                        std::string details = "ERROR :: The username-password combination is not recognized.";
                                        loginResult << LOGIN_FAILURE << SERVER << sender << details;

                                        if (!(*itr)->send(loginResult))
                                        {
                                            LOG("ERROR :: Unable to notify \'" + sender + "\' about invalid username-password combination they used to login.");
                                        }
                                    }
                                }

                                else
                                {
                                    LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                                }
                            }

                            else
                            {
                                LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                            }
                        }

                        else if (protocol == SIGNUP)
                        {
                            std::string sender, receiver, plainPassword;

                            if (packet >> sender >> receiver >> plainPassword)
                            {
                                if (receiver == SERVER)
                                {
                                    if (db.addNewUser(sender, plainPassword))
                                    {
                                        sf::Packet signupResult;
                                        signupResult << SIGNUP_SUCCESS << SERVER << sender;

                                        if ((*itr)->send(signupResult) != sf::Socket::Done)
                                        {
                                            LOG("ERROR :: Could not send signup acknowledgment to \'" + sender + "\'.");
                                        }

                                        m_selector.remove(**itr);
                                        itr = newConnections.erase(itr);
                                    }

                                    else
                                    {
                                        sf::Packet signupResult;
                                        std::string details = "ERROR :: The username \'" + sender + "\' exists.";
                                        signupResult << SIGNUP_FAILURE << SERVER << sender << details;

                                        if (!send(signupResult))
                                        {
                                            LOG("ERROR :: Unable to notify \'" + sender + "\' about unsuccessful attempt to sign up");
                                        }
                                    }
                                }

                                else
                                {
                                    LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                                }
                            }

                            else
                            {
                                LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                            }
                        }

                        else
                        {
                            LOG("ERROR :: An unknown protocol \'" + protocol + "\' is being tried be executed by the server.");
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet, requesting a new connection, was received by the server.");
                    }
                }

                else if (status == sf::Socket::Disconnected || status == sf::Socket::Error)
                {
                    m_selector.remove(**itr);
                    itr = newConnections.erase(itr);
                }
            }
        }
    }

    void Server::shutdown()
    {
        m_listener.close();
        LOG("Server successfully shutdown at " + getCurrentTimeAndDate() + " .");
    }
}
