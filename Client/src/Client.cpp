#include "../include/Client.hpp"
#include <regex>
#include <fstream>

namespace prattle
{
    Client::Client()
    {
        parseConfigFile();

        if (m_client_conf.ui == "gui")
            m_ui = std::make_shared<GraphicalUI>();

        else if (m_client_conf.ui == "cli")
            { /**/ }

        else
        {
            std::cout << "No UI set in config file. Using GUI by default." << std::endl;
            m_ui = std::make_shared<GraphicalUI>();
        }

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
                std::cout << "Login attempt" << std::endl;
                m_ui->setState(UserInterface::State::Connecting);
                /**/
                m_ui->setState(UserInterface::State::Chatting);
            }
            break;

            case UserInterface::UIEvent::UserSignup:
            {
                std::cout << "Signup attempt" << std::endl;
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
            int repliesCount = m_network.receive();
            while(repliesCount --> 0) //The Goes-To operator (c)
            {
                Network::Reply reply = m_network.popReply();
                //if (reply.type == Network::)
            }
            switch (m_state)
            {
                //STUFF !!
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
}
