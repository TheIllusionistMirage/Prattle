#include "../include/Client.hpp"
#include <regex>
#include <fstream>

namespace prattle
{
    Client::Client()
    {
        parseConfigFile();

        if (m_client_conf.ui == "gui")
            m_ui = std::unique_ptr<GraphicalUI>{new GraphicalUI{}};

        else if (m_client_conf.ui == "cli")
            { /**/ }

        else
        {
            std::cout << "No UI set in config file. Using GUI by default." << std::endl;
            m_ui = std::unique_ptr<GraphicalUI>{new GraphicalUI{}};
        }

        m_network.setIpAndPort(m_client_conf.addr, m_client_conf.port);
    }

    void Client::update()
    {
        UserInterface::UIEvent e = m_ui->update();

        switch (e)
        {
            case UserInterface::UIEvent::AddFriend:
            {
            }
            break;

            case UserInterface::UIEvent::Closed:
            {
                m_state = State::Exit;
                m_network.receive();
                m_network.disconnect();
            }
            break;

            case UserInterface::UIEvent::Disconnect:
            {
                m_network.disconnect();
                m_state = State::Login;
            }
            break;

            case UserInterface::UIEvent::MessageSent:
            {
            }
            break;

            case UserInterface::UIEvent::None:
            {
            }
            break;

            case UserInterface::UIEvent::Search:
            {
            }
            break;

            case UserInterface::UIEvent::UserLogin:
            {
                //std::cout << "Login attempt" << std::endl;

                /**/
                //m_ui->setState(UserInterface::State::Chatting);
                std::string uname = m_ui->getUsername();
                std::string paswd = m_ui->getPassword();

                std::cout << uname << " " << paswd << std::endl;

                if (m_network.connect())
                {
                    if (!isStringWhitespace(uname) && !isStringWhitespace(paswd))
                    {
                        m_ui->setState(UserInterface::State::Connecting);

                        int requestId = m_network.send(Network::TaskType::Login, std::vector<std::string>{uname, paswd});

                        if (requestId != 0)
                        {
                            LOG("LOG :: Login attempted on " + getCurrentTimeAndDate() + ". Awaiting server's reply.");
                        }
                    }
                    else
                    {
                        m_network.disconnect();
                        LOG("WARNING :: Attempting to use an empty string for either username or login or both.");
                    }
                }
                else
                {
                    LOG("ERROR :: Unable to connect to server! Please check your internet connection for any problems.");
                    m_network.reset();
                    m_ui->reset();
                }
            }
            break;

            case UserInterface::UIEvent::UserSignup:
            {
                //std::cout << "Signup attempt" << std::endl;
            }
            break;
        }
    }

    void Client::draw()
    {
        m_ui->draw();
    }

    void Client::run(float dt)
    {
        while (m_state != State::Exit)
        {
            if (m_network.isConnected())
            {
                int repliesCount = m_network.receive();

                while(repliesCount --> 0) //The Goes-To operator (c)
                {
                    Network::Reply reply = m_network.popReply();

                    if (reply.type == Network::ReplyType::TaskSuccess)
                    {
                        std::list<Network::Task> tasks = m_network.getPendingTasks();

                        for (auto& i : tasks)
                            if (reply.id == i.id)
                            {
                                m_network.popTask();

                                std::cout << "Logged in as " + reply.arguments[0] + " succesffuly." << std::endl;

                                std::cout << "Friendlist:-" << std::endl;
                                for (auto j = reply.arguments.begin() + 1; j != reply.arguments.end(); j++)
                                    std::cout << *j << " ";

                                m_ui->setState(UserInterface::State::Chatting);
                            }
                    }
                }
                switch (m_state)
                {
                    //STUFF !!
                }
            }

            update();
            draw();
        }
    }
    void Client::parseConfigFile()
    {
        std::ifstream configFile{m_configFilePath, std::ios::in};
        if (!configFile.is_open() || !configFile.good())
        {
            LOG("FATAL ERROR :: Error reading from \'" + m_configFilePath + "\'.");
            throw std::runtime_error("FATAL ERROR :: Error reading from \'" + m_configFilePath + "\'.");
        }

        static std::regex fieldPattern("(\\w+):([^:]+):"),
                            commentPattern("\\s*#.*");
        enum DataType { INT, STRING };

        //Maps the field name in the config file to (data type, pointer to variable) of the field
        std::map<std::string, std::pair<DataType,void*>> fieldsMap;
        fieldsMap.insert({"open_port", {INT, &m_client_conf.port}});
        fieldsMap.insert({"server_addr", {STRING, &m_client_conf.addr}});
        fieldsMap.insert({"ui", {STRING, &m_client_conf.ui}});
        std::string line;
        std::getline(configFile, line);

        for (unsigned int i = 1; !configFile.eof(); std::getline(configFile, line), i++)
        {
            std::string field;
            std::string value;
            std::smatch match;
            if(line.empty() || std::regex_match(line, commentPattern))
                continue;
            else if(std::regex_match(line, match, fieldPattern))
            {
                field = match[1].str();
                value = match[2].str();
            }
            else
            {
                LOG("Invalid field in config file : \n\t" + line);
                continue;
            }

            auto mapping = fieldsMap.find(field);
            if(mapping == fieldsMap.end())
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
    void Client::doLogin()
    {}
    void Client::doSignup()
    {}
    void Client::sendUserMessage()
    {}
    void Client::processServerReply()
    {}
    void Client::changeState(State s)
    {}

    bool Client::isStringWhitespace(const std::string& str)
    {
        for (auto& i : str)
        {
            if (i != ' ' && i != '\t' && i != '\n')
            {
                return false;
            }
        }
        return true;
    }
}
