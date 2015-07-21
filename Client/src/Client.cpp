#include "../include/Client.hpp"
#include <regex>
#include <fstream>

namespace prattle
{
    Client::Client() :
        m_state(UserInterface::State::Login)
    {
        parseConfigFile();

        if (m_clientConf.ui == "gui")
            m_ui = std::unique_ptr<GraphicalUI>{new GraphicalUI{}};
        //else if (m_clientConf.ui == "cli")
        //    m_ui = std::unique_ptr<decltype(m_ui)>(nullptr);
        else
        {
            ERR_LOG("No UI set in config file. Using GUI by default.");
            m_ui = std::unique_ptr<GraphicalUI>{new GraphicalUI{}};
        }

        changeState(UserInterface::State::Login);
    }

    void Client::run()
    {
        while (m_state != UserInterface::State::Exit)
        {
            update();
            draw();
        }
    }

    void Client::update()
    {
        //Poll network
        int networkUpdates = m_network.update();
        while (networkUpdates --> 0) //The Goes-To operator. See § 5.20/1 of the C++11 standard.
        {
            auto reply = m_network.popReply();
            switch (m_state)
            {
                case UserInterface::State::Login:
                case UserInterface::State::Signup:
                    //Not expecting any thing from network in these states
                    WRN_LOG("Received an unexpected network reply in Login/Signup state.");
                    break;
                case UserInterface::State::Connecting:
                    if (reply.id == m_loginReqId)
                    {
                        if (reply.type == Network::Reply::TaskSuccess)
                        {
                            int noOfFriends = std::stoi(reply.args[0]);
                            //DBG_LOG("User has " + reply.args[0] + " friends.");
                            std::vector<std::string> friends;

                            for (int i = 1; i <= noOfFriends; i++ )
                            {

                                friends.push_back(reply.args[i]);
                                m_chatHistory[friends.back()] = "";
                            }

                            m_ui->fillFriendList(friends);

                            changeState(UserInterface::State::Chatting);
                        }
                        else if (reply.type == Network::Reply::TaskError)
                        {
                            if (m_network.isConnected())
                                m_ui->alert("Wrong username/password combination!");
                            else
                                m_ui->alert("Unable to connect to server!");
                            changeState(UserInterface::State::Login);
                            m_network.reset();
                        }
                        else
                        {
                            //todo: look at the reply and display the specific error/issue through the ui

                            m_ui->alert("Unable to login!");
                            changeState(UserInterface::State::Login);
                            m_network.reset();
                        }
                    }
                    else if (reply.id == m_signupReqId)
                    {
                        if (reply.type == Network::Reply::TaskSuccess)
                        {
                            m_network.reset();
                            m_ui->reset();
                            changeState(UserInterface::State::Login);
                            m_ui->alert("Signup successful! Login to start chatting!");
                        }
                        else
                        {
                            //todo: look at the reply and display the specific error/issue through the ui

                            m_ui->alert("Unable to Signup!");
                            changeState(UserInterface::State::Signup);
                            m_network.reset();
                        }
                    }
                    else
                        WRN_LOG("Received an unexpected network reply in state connecting.");
                    break;
                case UserInterface::State::Chatting:
                    {
                        // Code goes here ! (And I mean a lot of it)
                        if (reply.id == Network::InvalidRequest)
                        {
                            // Actually, when the user receives a message,
                            // he receives no request id sicne the task
                            // wasn't created on his side. So, we had no
                            // choice but to use invalidrequest as request
                            // id in case when the user receives a message
                            // from another remote client.

                            if (reply.type == Network::Reply::RecievedMessage)
                            {
                                m_ui->addToChatArea(reply.args[0] + " : " + reply.args[1]);
                                //m_chatHistory[m_ui->getUsername()] =
                                m_chatHistory.find(reply.args[0])->second = m_chatHistory.find(reply.args[0])->second + m_ui->getUsername() + " : " + reply.args[1] + "\n";
                            }
                            else if (reply.type == Network::Reply::OnlineNotif)
                            {
                                m_ui->setStatusOfFriend(reply.args[0], 1);  // remember from GraphicListItem class, 0 is for offline, 1 is for online textures.
                            }
                        }

                        if (m_unsentMsgReqId.size() > 0)
                        {
                            auto msgId = std::find(m_unsentMsgReqId.begin(), m_unsentMsgReqId.end(), reply.id);
                            if (msgId != m_unsentMsgReqId.end())
                            {
                                if (reply.type == Network::Reply::TaskSuccess)
                                {
                                    m_ui->addToChatArea(m_ui->getUsername() + " : " + m_ui->getInputText().substr(0, m_ui->getInputText().length() - 1));
                                    m_chatHistory.find(m_ui->getSelectedFriend())->second = m_chatHistory.find(m_ui->getSelectedFriend())->second + m_ui->getUsername() + " : " + m_ui->getInputText().substr(0, m_ui->getInputText().length() - 1) + "\n";
                                    m_ui->setInputText("");
                                    DBG_LOG("Successfully sent message");
                                }
                            }
                            else if (msgId == m_unsentMsgReqId.end())
                            {
                                m_ui->alert("Unable to send message to \'" + m_ui->getSelectedFriend() + "\'!");
                            }
                        }
                        else
                            WRN_LOG("Received an unexpected network reply in state Chatting. Received reply : " + std::to_string(reply.id) + " " + std::to_string(reply.type));
                    }
                    break;
                case UserInterface::State::Exit:
                    // This should just never happen, but defining it to prevent GCC warning ...
                    WRN_LOG("LOLZ, this should never have happened. Go pray to the Flying Spaghetti Monster :p");
                    break;
            }
        }

        //Poll/update UI
        auto event = m_ui->update();
        if (event == UserInterface::UIEvent::Closed)
            m_state = UserInterface::State::Exit;
        else if (event == UserInterface::UIEvent::StateChanged)
        {
            changeState(m_ui->getState());
            DBG_LOG("State changed.");
        }
        else if (event != UserInterface::UIEvent::None)
        {
            switch (m_state)
            {
                case UserInterface::State::Login:
                    if (event == UserInterface::UIEvent::UserLogin)
                    {
                        if (!m_ui->isStringWhitespace(m_ui->getUsername()) &&
                             !m_ui->isStringWhitespace(m_ui->getPassword()))
                        {
                            changeState(UserInterface::State::Connecting);
                            m_loginReqId = m_network.send(Network::Task::Login, {
                                           m_clientConf.addr,
                                           std::to_string(m_clientConf.port),
                                           m_ui->getUsername(),
                                           m_ui->getPassword() });
                        }
                        else
                            m_ui->alert("Can't leave either login fields blank!");
                    }
                    else
                        WRN_LOG("Unexpected UIEvent received in Login State. Event code: " + std::to_string(event));
                    break;
                case UserInterface::State::Signup:
                    if (event == UserInterface::UIEvent::UserSignup)
                    {
                        if (!m_ui->isStringWhitespace(m_ui->getUsername()) &&
                                 !m_ui->isStringWhitespace(m_ui->getPassword()))
                            {
                                changeState(UserInterface::State::Connecting);
                                m_signupReqId = m_network.send(Network::Task::Signup, {
                                               m_clientConf.addr,
                                               std::to_string(m_clientConf.port),
                                               m_ui->getUsername(),
                                               m_ui->getPassword() });
                                DBG_LOG("Signup event.");
                            }
                            else
                                m_ui->alert("Can't leave either signup fields blank!");
                    }
                    else
                        WRN_LOG("Unexpected UIEvent received in Signup State. Event code: " + std::to_string(event));
                    break;
                case UserInterface::State::Connecting:
                case UserInterface::State::Chatting:
                    switch (event)
                    {
                        case UserInterface::UIEvent::Disconnect:
                            m_network.send(Network::Task::Type::Logout);
                            //m_network.reset();
                            m_ui->reset();
                            changeState(UserInterface::State::Login);
                            m_unsentMsgReqId.clear();
                            m_chatHistory.clear();
                            break;
                        case UserInterface::UIEvent::MessageSent:
                            m_unsentMsgReqId.push_back(m_network.send(Network::Task::SendMsg, {
                                           m_ui->getSelectedFriend(),
                                           m_ui->getInputText().substr(0, m_ui->getInputText().length() - 1) }));
                                           //m_ui->getInputText().substr(0, m_ui->getInputText().length() - 1)
                            //m_ui->addToChatArea(m_ui->getUsername() + " : " + m_ui->getInputText());
//                            m_ui->addToChatArea(m_ui->getUsername() + " : " + m_ui->getInputText().substr(0, m_ui->getInputText().length() - 1));
//                            m_ui->setInputText("");
                            //DBG_LOG("Message sent to server.");
                            break;
                        case UserInterface::UIEvent::TabSelected:
                            //
                            m_ui->clearChat();
                            //std::cout << "Selected friend : " << m_ui->getSelectedFriend() << std::endl;
                            m_ui->addToChatArea(m_chatHistory.find(m_ui->getSelectedFriend())->second);
                            DBG_LOG(m_chatHistory.find(m_ui->getSelectedFriend())->second);
                            break;
                        default:
                            WRN_LOG("Unhandled or unexpected UIEvent received in Chatting state.");
                    }
                    break;
                case UserInterface::State::Exit:
                    // This should just never happen, but defining it to prevent GCC warning ...
                    WRN_LOG("We should've prayed to the Flying Spaghetti Monster harder, sorry.");
                    break;
            }
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
            ERR_LOG("FATAL ERROR :: Error reading from \'" + m_configFilePath + "\'.");
            throw std::runtime_error("FATAL ERROR :: Error reading from \'" + m_configFilePath + "\'.");
        }

        const std::regex fieldPattern("(\\w+):([^:]+):"),
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
    void Client::changeState(UserInterface::State s)
    {
        m_state = s;
        m_ui->setState(s);
    }
}
