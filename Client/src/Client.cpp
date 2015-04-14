#include "../include/Client.hpp"

namespace prattle
{
    Client::Client() : m_username{""}
                     , m_password{""}
                     , m_friends{}
                     , m_socket{}
    {
        readConfigFile();
    }

    void Client::readConfigFile()
    {
        std::ifstream configFile{CONFIG_FILE, std::ios::in};

        if(!configFile.is_open() || !configFile.good())
        {
            LOG("FATAL ERROR :: Error reading from \'" + CONFIG_FILE + "\'.");
            throw std::runtime_error("FATAL ERROR :: Error reading from \'" + CONFIG_FILE + "\'.");
        }

        configFile.seekg(std::ios::beg);
        std::string line;
        std::getline(configFile, line);

        for (unsigned int i = 1; !configFile.eof(); std::getline(configFile, line), i++)
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
                LOG("WARNING :: Invalid configuration value at " + CONFIG_FILE + " : " + std::to_string(i));
                continue;
            }

            field = line.substr(0, first_colon);
            value = line.substr(first_colon + 1, second_colon - first_colon - 1);

            if (field == "SERVER_IP")
                m_clientConf.ip = value;

            else if (field == "OPEN_PORT")
                m_clientConf.port = std::stoi(value);
        }

        configFile.close();
    }

    void Client::resetSocket()
    {
        disconnect();
        setStatus(Status::Offline);
    }

    bool Client::checkIfWhitespace(const std::string& message)
    {
        for (auto itr = message.begin(); itr != message.end(); itr++)
        {
            if (*itr != ' ' && *itr != '\t' && *itr != '\n')
            {
                return false;
            }
        }
        return true;
    }

    void Client::setStatus(Status status)
    {
        m_status = status;
    }

    Status Client::getStatus()
    {
        return m_status;
    }

    bool Client::send(const sf::Packet& packet)
    {
        sf::Packet packetCopy{packet};
        std::string protocol;

        if (packetCopy >> protocol)
        {
            if ( protocol == LOGIN       ||
                  protocol == SIGNUP      ||
                   protocol == SEND_MSG    ||
                    protocol == SEARCH_USER ||
                     protocol == ADD_FRIEND  ||
                      protocol == NOTIF_LOGIN ||
                       protocol == NOTIF_LOGOUT )
            {
                std::string sender, receiver, msg;
                packetCopy >> sender >> receiver >> msg;

                return m_socket.send(packetCopy) == sf::Socket::Status::Done;
            }

            else
            {
                LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                return false;
            }
        }

        else
        {
            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
            return false;
        }
    }

    bool Client::receive(sf::Packet& packet)
    {
        sf::Socket::Status status = m_socket.receive(packet);
        sf::Packet packetCopy{packet};

        if (status == sf::Socket::NotReady)
            return false;

        else if (status == sf::Socket::Done)
        {
            std::string protocol;

            if (packetCopy >> protocol)
            {
                if (protocol == LOGIN_SUCCESS)
                {
                    std::string sender, user, frnd;
                    unsigned short friendCount;
                    std::vector<std::string> friends;

                    if (packetCopy >> sender >> user >> friendCount)
                    {
                        if (sender == SERVER)
                        {
                            for (auto i = 1; i <= 2; ++i)
                            {
                                packetCopy >> frnd;
                                friends.push_back(frnd);
                            }

                            LOG("Client \'" + user + "\' successfully logged in.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == LOGIN_FAILURE)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user >> details)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Server to Client :: " + details);
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SIGNUP_SUCCESS)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Client \'" + user + "\' successfully signed up.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SIGNUP_FAILURE)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user >> details)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Server to Client :: " + details);
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SEND_MSG)
                {
                    std::string sender, source, user, message;

                    if (packetCopy >> sender >> source >> user >> message)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Client \'" + user + "\' successfully received packet.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SEND_MSG_SUCCESS)
                {
                    std::string sender, user;

                    if (packetCopy >> sender >> user)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Client \'" + user + "\' successfully sent packet.");
                            //std::cout << "returning true" << std::endl;
                            return true;
                            //std::cout << "returned" << std::endl;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SEND_MSG_FAILURE)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user >> details)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Server to Client :: " + details);
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == SEARCH_USER_RESULTS)
                {
                    std::string sender, user, result;

                    if (packetCopy >> sender >> user >> result)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Successfully obtained details from server about user \'" + result + "\'.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == ADD_FRIEND_SUCCESS)
                {
                    std::string sender, user;

                    if (packetCopy >> sender >> user)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Client \'" + user + "\' successfuly added a friend.");
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == ADD_FRIEND_FAILURE)
                {
                    std::string sender, user, details;

                    if (packetCopy >> sender >> user >> details)
                    {
                        if (sender == SERVER)
                        {
                            LOG("Server to Client :: " + details);
                            return true;
                        }

                        else
                        {
                            LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                            return false;
                        }
                    }

                    else
                    {
                        LOG("ERROR :: A damaged packet is being tried to be sent by the client.");
                        return false;
                    }
                }

                else if (protocol == NOTIF_LOGIN)
                {
                    std::string source, receiver, sender;

                    if (packetCopy >> source >> receiver >> sender)
                    {
                        LOG("New notification about login of \'" + sender + "\' received");
                        return true;
                    }

                    else
                    {
                        LOG("Error in receiving notification/packet is corrupt");
                        return false;
                    }
                }

                else if (protocol == NOTIF_LOGOUT)
                {
                    std::string source, receiver, sender;

                    if (packetCopy >> source >> receiver >> sender)
                    {
                        LOG("New notification about logout of \'" + sender + "\' received");
                        return true;
                    }

                    else
                    {
                        LOG("Error in receiving notification/packet is corrupt");
                        return false;
                    }
                }

                else if (protocol == NOTIF_ONLINE)
                {
                    std::string source, receiver, onlineFriend;

                    std::cout << "o" << std::endl;

                    if (packetCopy >> source >> receiver >> onlineFriend)
                    {
                        std::cout << "oo" << std::endl;
                        LOG("New notification for \'" + onlineFriend + "\' received");
                        return true;
                    }

                    else
                    {
                        std::cout << "ooo" << std::endl;
                        LOG("Error in receiving notification/packet is corrupt");
                        return false;
                    }
                }

                else
                {
                    LOG("ERROR :: An unknown protocol \'" + protocol + "\' is being tried be executed by the client.");
                }
            }
        }

        else if (status == sf::Socket::Error)
            LOG("ERROR :: Unable to receive data from remote peer!");

        return false;
    }

    bool Client::isLoggedIn()
    {
        return m_status == Status::Online;// || m_status == Status::Away;
    }

    void Client::blockSocket(bool blocking)
    {
        m_socket.setBlocking(blocking);
    }

    void Client::disconnect()
    {
        blockSocket(true);
        m_status = Status::Offline;
        m_socket.disconnect();
    }

    bool Client::connect()
    {
        return m_socket.connect(m_clientConf.ip, m_clientConf.port, sf::milliseconds(5000)) == sf::Socket::Status::Done;
    }
}
