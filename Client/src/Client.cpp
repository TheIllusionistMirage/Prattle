#include "../include/Client.hpp"
#include <regex>
#include <fstream>

namespace prattle
{
    Client::Client() :
        m_state(State::Login)
    {
        parseConfigFile();

        if (m_clientConf.ui == "gui")
            m_ui = std::unique_ptr<GraphicalUI>{new GraphicalUI{}};
//        else if (m_clientConf.ui == "cli")
            //m_ui = std::unique_ptr<decltype(m_ui)>(nullptr);
        else
        {
            LOG("No UI set in config file. Using GUI by default.");
            m_ui = std::unique_ptr<GraphicalUI>{new GraphicalUI{}};
        }

    }

    void Client::run()
    {
        while (m_state != State::Exit)
        {
            update();
            draw();
        }
    }

    void Client::update()
    {
        int updates = m_network.receive();
        while (updates --> 0)
        {
            auto reply = m_network.popReply();
            if (reply.id == m_loginReqId)
            {
                if (reply.type == Network::Reply::TaskSuccess)
                {
                    m_ui->setState(UserInterface::State::Chatting);
                    m_state = State::Chatting;
                }
                else
                {
                    //todo: look at the reply and display the specific error/issue in the ui
                    std::cout << "Login failed. Exiting because I don't know how to use ui dialogs." << std::endl;
                    m_state = State::Exit;
                }
            }
        }

        //UI Event
        UserInterface::UIEvent e = m_ui->update();
        switch (e)
        {
            case UserInterface::UIEvent::AddFriend:
            break;
            case UserInterface::UIEvent::Closed:
            {
                m_state = State::Exit;
            }
            break;
            case UserInterface::UIEvent::Disconnect:
            {
                m_ui->setState(UserInterface::State::Login);
                m_network.send(Network::Task::Type::Logout);
                m_state = State::Login;
            }
            break;
            case UserInterface::UIEvent::MessageSent:
            break;
            case UserInterface::UIEvent::None:
            break;
            case UserInterface::UIEvent::Search:
            break;
            case UserInterface::UIEvent::UserLogin:
            {
                if (m_state != State::Login)
                    break;

                m_ui->setState(UserInterface::State::Connecting);
                m_state = State::Connecting;
                m_loginReqId = m_network.send(Network::Task::Login, {
                               m_clientConf.addr,
                               std::to_string(m_clientConf.port),
                               m_ui->getUsername(),
                               m_ui->getPassword()} );
            }
            break;
            case UserInterface::UIEvent::UserSignup:
                std::cout << "Signup" << std::endl;
            break;
        }
    }

    void Client::draw()
    {
        m_ui->draw();
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
        fieldsMap.insert({"open_port", {INT, &m_clientConf.port}});
        fieldsMap.insert({"server_addr", {STRING, &m_clientConf.addr}});
        fieldsMap.insert({"ui", {STRING, &m_clientConf.ui}});

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
