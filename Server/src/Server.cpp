#include "Server.hpp"

namespace prattle
{
    Server::Server():
        timeOut(sf::seconds(60))
    {
        auto status = m_listener.listen(OPEN_PORT);
        LOG("sf::TcpListener object now listening to port " + std::to_string(OPEN_PORT));

        if(status != sf::Socket::Done)
        {
            LOG("FATAL ERROR : Error binding the listener at " + std::to_string(OPEN_PORT));
            throw std::runtime_error("Fatal error : Error binding the listener at " + std::to_string(OPEN_PORT));
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
        std::cout << "--- Listening to incoming connections at port " << OPEN_PORT << " ---" << std::endl << std::endl;
        std::cout << "--- SERVER LOG ---" << std::endl << std::endl;

        LOG("Server went up at : " + getCurrentTimeAndDate());
    }

    void Server::run()
    {
        while (isRunning())
        {
            //db.reloadAllRecords();

            LOG(std::to_string(m_messages.size()));
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

            //std::cout << db.getRecord("raptor").friends.back() << std::endl;

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
            std::cout << "Protocol : " + protocol;
            if ( protocol == LOGIN_SUCCESS       ||
                  protocol == LOGIN_FAILURE       ||
                   protocol == SIGNUP_SUCCESS      ||
                    protocol == SIGNUP_FAILURE      ||
                     protocol == SEND_MSG            ||
                      protocol == SEND_MSG_SUCCESS    ||
                       protocol == SEND_MSG_FAILURE    ||
                        protocol == SEARCH_USER_RESULTS ||
                         protocol == ADD_FRIEND_SUCCESS  ||
                          protocol == ADD_FRIEND_FAILURE   )
            {
                std::string sender, username;

                if (packetCopy >> sender >> username)       // NOTE : the sender in this case is ALWAYS the server.
                {
                    std::cout << "Mesg being sent to " + username << std::endl;
                    auto itr = m_clients.find(username);
                    bool result = true;

                    if (itr == m_clients.end())
                    {
                        // NOTE : the sender in this case is ALWAYS the server.
                        m_messages.insert(std::make_pair(username, std::make_pair(sender, packetCopy)));
                        LOG("Msg for offline user " + username + "Stored in server");
                    }

                    else
                    {
                        LOG("Msg for online user " + username + "sent");
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
                //std::string sender, receiver, msg;

                auto status = (itr->second)->receive(packet);

                if (status == sf::Socket::Done)
                {
                    /*if (dataPacket >> sender >> receiver >> msg)
                    {
                        // beware hacky, dirty code here
                        if (receiver == "server")
                        {
                            searchDatabase(msg, sender);
                        }

                        else
                        {
                            sf::Packet msgPacket;
                            msgPacket << sender << msg;

                            if (!send(sender, receiver, msgPacket))
                                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: Error in sending message to user!" << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: Improper packet!" << std::endl;
                    }*/
                    std::string protocol;

                    if (packet >> protocol)
                    {
                        if ( //protocol == LOGIN       ||
                              //protocol == SIGNUP      ||
                               protocol == SEND_MSG)//    ||
                                //protocol == SEARCH_USER ||
                                 //protocol == ADD_FRIEND    )
                        {
                            std::string sender, receiver, data;

                            //std::cout << protocol << std::endl;
                            if (packet >> sender >> receiver >> data)
                            {
                                /*if (receiver == SERVER)
                                {
                                    //
                                }*/

                                std::cout << protocol << receiver << sender << data<< std::endl;
                                //else
                                {
                                    sf::Packet newPacket;
                                    newPacket << SEND_MSG << SERVER << receiver << sender << data;
                                    LOG(SEND_MSG + SERVER + sender + receiver + data);

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
                            }

                            else
                            {
                                LOG("ERROR :: A damaged packet was received by the server from \'" + itr->first + "\'.");
                            }
                        }

                        else if ( //protocol == LOGIN       ||
                              //protocol == SIGNUP      ||
                               //protocol == SEND_MSG)   ||
                                protocol == SEARCH_USER )//||
                                 //protocol == ADD_FRIEND    )
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

                        else if ( //protocol == LOGIN       ||
                              //protocol == SIGNUP      ||
                               //protocol == SEND_MSG)   ||
                                //protocol == SEARCH_USER )||
                                 protocol == ADD_FRIEND    )
                        {
                            std::string sender, receiver, user;

                            if (packet >> sender >> receiver >> user)
                            {
                                if (receiver == SERVER)
                                {
                                    if (db.isUserRegistered(user))
                                    {
                                        std::cout << "adding friend : " + user << std::endl;
                                        db.addNewFriend(sender, user);

                                        sf::Packet result;
                                        result << ADD_FRIEND_SUCCESS << SERVER << sender;

                                        if (!send(result))
                                        {
                                            LOG("ERROR :: Failed to send success info to \'" + sender + "\' for successfully adding \'" + user + "\' as a friend.");
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
                    std::cout << " x [" + itr->first + "] left chat " << getCurrentTimeAndDate() << std::endl;
                    LOG("[" + itr->first + "] left chat on " + getCurrentTimeAndDate());
                    m_selector.remove(*itr->second);
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
                        if (protocol == LOGIN) //||
                             //protocol == SIGNUP)
                        {
                            std::string sender, receiver, plainPassword;

                            if (packet >> sender >> receiver >> plainPassword)
                            {
                                std::cout << protocol + " " + sender+" "+ receiver +" "+ plainPassword << std::endl;
                                if (receiver == SERVER)
                                {
                                    std::cout << "x" << std::endl;
                                    if (db.isValidPassword(sender, plainPassword))
                                    {
                                        std::cout << "xx" << std::endl;
                                        sf::Packet loginResult;
                                        loginResult << LOGIN_SUCCESS << SERVER << sender << db.getRecord(sender).friends.size();
                                        std::cout << "9 " << db.getRecord(sender).friends.size() << " 9" << std::endl;
                                        //std::cout << "9 " << loginResult.getDataSize() << " 9" << std::endl;

                                        for (auto& friendName : db.getRecord(sender).friends)
                                        {
                                            loginResult << friendName;
                                            //std::cout << friendName << std::endl;
                                        }
                                        //std::cout << "9 " << loginResult.getDataSize() << " 9" << std::endl;

                                        //
                                        /*sf::Packet p{loginResult};

                                        std::string pro, sen, us, fri;
                                        unsigned int friCnt;

                                        if (p >> pro >> sen >> us  >> friCnt)
                                        {
                                            if (pro == LOGIN_SUCCESS)
                                            {
                                                std::cout << pro << " " << sen << " " << us << " " << friCnt;
                                                for (auto i = 1; i <= friCnt; ++i)
                                                {
                                                    p >> fri;
                                                    std::cout << " " << fri;
                                                    //friends.push_back(frnd);
                                                }
                                            }
                                        }*/
                                        //

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
                                            LOG("Message not sent as user is online ........");
                                            if ((*itr)->send(itr_2->second.second) != sf::Socket::Done)
                                            {
                                                LOG("ERROR :: Error in sending packet from " + itr_2->first + " to " + sender + ".");
                                            }
                                        }

                                        m_messages.erase(sender);
                                        m_clients.insert(std::make_pair(sender, std::move(*itr)));
                                        itr = newConnections.erase(itr);
                                    }

                                    else
                                    {
                                        std::cout << "failed" << std::endl;
                                        sf::Packet loginResult;
                                        std::string details = "ERROR :: The username-password combination is not recognized.";
                                        loginResult << LOGIN_FAILURE << SERVER << sender << details;

                                        //if (!send(loginResult))
                                        if (!(*itr)->send(loginResult))
                                        {
                                            std::cout << "failed ** " << std::endl;
                                            LOG("ERROR :: Unable to notify \'" + sender + "\' about invalid username-password combination they used to login.");
                                        }

                                        //else
                                        //    LOG("ERROR :: Notified \'" + sender + "\' about invalid username-password combination they used to login.");
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

    /*void Server::searchDatabase(const std::string& username)
    {
        if (db.isUserRegistered(username))
        {
            sf::Packet result;
            result << SEARCH_USER_RESULTS << SERVER << ;

            if (!send("server", resultReceiver, result))
            {
                std::cerr << __FILE__ << ":" << __LINE__ << "  ERROR :: Unable to send search results to " << resultReceiver << std::endl;
            }
        }
    }*/

    void Server::shutdown()
    {
        m_listener.close();
        LOG("Server successfully shutdown at " + getCurrentTimeAndDate() + " .");
    }
}
