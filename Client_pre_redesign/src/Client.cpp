#include "../include/Client.hpp"
#include <regex>

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

        static std::regex field_pattern("(\\w+):([[:print:]]+):"),
                            comment_pattern("\\s*#.*");
        enum data_type { INT, STRING };
        //Maps the field name in the config file to (data type, pointer to variable) of the field
        std::map<std::string, std::pair<data_type,void*>> fields_map;
        fields_map.insert({"open_port", {INT, &m_client_conf.port}});
        fields_map.insert({"server_addr", {STRING, &m_client_conf.addr}});
        std::string line;
        std::getline(configFile, line);

        for (unsigned int i = 1; !configFile.eof(); std::getline(configFile, line), i++)
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
                LOG("Invalid field in config file : \n\t" + line);
                continue;
            }

            auto mapping = fields_map.find(field);
            if(mapping == fields_map.end())
            {
                LOG("Warning : Unrecognized field in conifg file, ignoring.");
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
                    LOG("Unhandled data type");
                    break;
                }
            }
        }
    }

    void Client::resetSocket()
    {
        disconnect();
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
        return m_socket.getRemotePort() != 0;
    }

    void Client::blockSocket(bool blocking)
    {
        m_socket.setBlocking(blocking);
    }

    void Client::disconnect()
    {
        blockSocket(true);
        m_socket.disconnect();
    }

    bool Client::connect()
    {
        return m_socket.connect(m_client_conf.addr, m_client_conf.port, sf::milliseconds(5000)) == sf::Socket::Status::Done;
    }
}
