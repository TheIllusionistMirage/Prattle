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
            ERR_LOG("No UI set in config file. Using GUI by default.");
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
        //Poll network
        int networkUpdates = m_network.receive();
        while (networkUpdates --> 0) //The Goes-To operator. See § 5.20/1 of the C++11 standard.
        {
            auto reply = m_network.popReply();
            switch (m_state)
            {
                case State::Login:
                case State::Signup:
                    //Not expecting any thing from network in these states
                    WRN_LOG("Received an unexpected network reply in Login/Signup state.");
                    break;
                case State::Connecting:
                    if (reply.id == m_loginReqId)
                    {
                        if (reply.type == Network::Reply::TaskSuccess)
                        {
                            m_ui->setState(UserInterface::State::Chatting);
                            m_state = State::Chatting;
                        }
                        else
                        {
                            //todo: look at the reply and display the specific error/issue through the ui
                            std::cout << "Login failed. Exiting because I don't know how to use ui dialogs." << std::endl;
                            m_state = State::Exit;
                        }
                    }
                    else
                        WRN_LOG("Received an unexpected network reply in state connecting.");
                    break;
                case State::Chatting:
                    // Code goes here ! (And I mean a lot of it.
                    break;
                case State::Exit:
                    //This should never happen
                    WRN_LOG("lolz ur codz r broke.");
                    break;
            }
        }

        //Poll/update UI
        auto event = m_ui->update();
        switch (m_state)
        {
            case State::Login:
                if (event == UserInterface::UIEvent::UserLogin)
                {
                    m_ui->setState(UserInterface::State::Connecting);
                    m_state = State::Connecting;
                    m_loginReqId = m_network.send(Network::Task::Login, {
                                   m_clientConf.addr,
                                   std::to_string(m_clientConf.port),
                                   m_ui->getUsername(),
                                   m_ui->getPassword() });

                }
                else if (event != UserInterface::UIEvent::Closed)
                    WRN_LOG("Unexpected UIEvent received in Login State. Event code: " + std::to_string(event));
                break;
            case State::Signup:
                break;
            case State::Connecting:
            case State::Chatting:
                switch (event)
                {
                    case UserInterface::UIEvent::Disconnect:
                        m_ui->setState(UserInterface::State::Login);
                        m_network.send(Network::Task::Type::Logout);
                        m_state = State::Login;
                        break;
                    case UserInterface::UIEvent::Closed:
                        break;
                    default:
                        WRN_LOG("Unhandled or unexpected UIEvent received in Chatting state.");
                }
                break;
            case State::Exit:
                //This should never happen
                WRN_LOG("lolz ur codz r broke.");
                break;
        }
        //Whatever state the application is in, Closing always follows exiting
        if (event == UserInterface::UIEvent::Closed)
            m_state = State::Exit;
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
            ERR_LOG("FATAL ERROR :: Error reading from \'" + m_configFilePath + "\'.");
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
                WRN_LOG("Invalid field in config file : \n\t" + line);
                continue;
            }

            auto mapping = fieldsMap.find(field);
            if(mapping == fieldsMap.end())
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
