#include "../include/Client.hpp"
#include <regex>
#include <fstream>

namespace prattle
{
    Client::Client()
    {
    }
    void Client::run()
    {
        while (m_state != Exit)
        {
            switch (m_state)
            {
                //STUFF !!
            }
            int repliesCount = m_network.receive();
            while(repliesCount --> 0) //The Goes-To operator (c)
            {
                Network::Reply reply = m_network.popReply();
                if (reply.type == Network::)
            }
        }
    }
    void parseConifgFile()
    {
        std::ifstream configFile{m_configFilePath, std::ios::in};
        if (!configFile.is_open() || !configFile.good())
        {
            LOG("FATAL ERROR :: Error reading from \'" + CONFIG_FILE + "\'.");
            throw std::runtime_error("FATAL ERROR :: Error reading from \'" + CONFIG_FILE + "\'.");
        }

        static std::regex fieldPattern("(\\w+):([^:]+):"),
                            commentPattern("\\s*#.*");
        enum DataType { INT, STRING };

        //Maps the field name in the config file to (data type, pointer to variable) of the field
        std::map<std::string, std::pair<DataType,void*>> fieldsMap;
        fieldsMap.insert({"open_port", {INT, &m_client_conf.port}});
        fieldsMap.insert({"server_addr", {STRING, &m_client_conf.addr}});
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
    void doLogin()
    {}
    void doSignup()
    {}
    void sendUserMessage()
    {}
    void processServerReply()
    {}
    void changeState(State s)
    {}
}
