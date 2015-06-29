/**

    Prattle/Server/UserDatabase.hpp
    ===============================

    Contains implementations of class UserDatabase declared in Prattle/Server/UserDatabase.hpp

*/

#include "../include/UserDatabase.hpp"
#include <regex>

namespace prattle
{
    UserDatabase::UserDatabase()
    {
        parseFile();
    }

    bool UserDatabase::isUserRegistered(const std::string& username)
    {
        return records.find(username) != records.end();
    }

    const std::vector<std::string>& UserDatabase::getFriends(const std::string& username)
    {
        auto itr = records.find(username);

        if (itr == records.end())
        {
            LOG("Invalid username queried at function getFriends()");
            throw std::runtime_error("Invalid username queried at function getFriends()");
        }

        return itr->second.friends;
    }

    bool UserDatabase::isValidPassword(const std::string& username,const std::string& plain_pwd)
    {
        auto res = records.find(username);

        if(res == records.end())
            return false;

        return res->second.hashed_pwd == pwd_hash(plain_pwd,res->second.salt);
    }

    bool UserDatabase::addNewUser(const std::string &username,const std::string& plain_pwd)
    {
        if(isUserRegistered(username))
        {
            return false;
        }
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);
        if(dbFile.bad())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        Record newRecord{"", generate_salt()};
        newRecord.hashed_pwd = pwd_hash(plain_pwd, newRecord.salt);
        records.insert({username, newRecord});
        dbFile.seekp(0, std::ios::end);

        if(!(dbFile << username << ':' << newRecord.hashed_pwd << ':' << newRecord.salt << ':' << std::endl))
        {
            LOG("FATAL ERROR :: Error in writing a new recored to database " + USER_LIST + "!");
            throw std::runtime_error("Error while writing new record to " + USER_LIST);
        }
        dbFile.close();

        return true;
    }

    bool UserDatabase::addNewFriend(const std::string& username, const std::string& friendname)
    {
        if (isUserRegistered(username) && isUserRegistered(friendname))
        {
            auto& user_rec = records[username];
            auto& friend_rec = records[friendname];

            user_rec.friends.push_back(friendname);
            friend_rec.friends.push_back(username);

            updateRecordOnFile(username);
            updateRecordOnFile(friendname);

            return true;
        }
        else
        {
            LOG("ERROR :: Either " + username + " or " + friendname + " is not registered!");
            return false;
        }

        return false;
    }

    void UserDatabase::printAllRecords()
    {
        for(auto& r : records)
        {
            std::cout << r.first << '\n'
                       << '\t' << r.second.hashed_pwd << '\n'
                       << '\t' << r.second.salt << '\n';
            std::cout << '\t';
            for(auto& f : r.second.friends)
                std::cout << f << ',';
            std::cout << "\n\n";
        }
    }

    bool UserDatabase::removeUser(const std::string& username)
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);
        if(dbFile.bad())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        std::stringstream temp;
        std::string line;
        dbFile.seekg(std::ios::beg);
        dbFile.seekp(std::ios::beg);
        std::getline(dbFile,line);

        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile, line) , ++line_num)
        {
            auto first_colon = line.find(':', 0);

            if(first_colon != std::string::npos && line.substr(0, first_colon) == username)
            {
                continue;
            }
            else
            {
                temp << line << std::endl;
            }
        }

        dbFile.close();
        dbFile.open(USER_LIST, std::ios::out | std::ios::trunc);
        dbFile << temp.str();
        dbFile.close();
        return true;
    }

    bool UserDatabase::updateRecordOnFile(const std::string& username)
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);
        if(dbFile.bad())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        std::stringstream temp;
        std::string line;
        dbFile.seekg(std::ios::beg);
        dbFile.seekp(std::ios::beg);
        std::getline(dbFile,line);
        auto& record = records[username];

        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile, line) , ++line_num)
        {
            auto first_colon = line.find(':', 0);

            if(first_colon != std::string::npos && line.substr(0, first_colon) == username)
            {
                temp << username << ':' << record.hashed_pwd << ':' << record.salt << ':';

                if (record.friends.size() > 0)
                {
                    for (auto &itr : record.friends)
                    {
                        temp << itr << ',';
                    }

                    temp << ':' << std::endl;
                }
            }

            else
            {
                temp << line << std::endl;
            }
        }

        dbFile.close();
        dbFile.open(USER_LIST, std::ios::out | std::ios::trunc);
        dbFile << temp.str();
        dbFile.close();
        return true;
    }

    void UserDatabase::parseFile()
    {
        static std::regex record_pattern("
                ([\\w.]{3,16}):([0-9a-f]{64}):([[:alnum:]]+):(?:([\\w.]{3,16}),)*:"),
                comment_pattern("\\s*#.*");

        dbFile.open(USER_LIST, std::ios_base::in);
        if(dbFile.bad())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }
        records.clear();
        std::string line;
        std::getline(dbFile,line);
        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile, line) , ++line_num)
        {
            std::string username, hashed_pwd, salt;
            std::vector<std::string> friends;
            std::smatch record_match;
            if(line.empty() || std::regex_match(line, comment_pattern))
                continue;
            else if(std::regex_match(line, record_match, record_pattern))
            {
                username = record_match[1].str();
                hashed_pwd = record_match[2].str();
                salt = record_match[3].str();

                //for(std::size_t i = 4; record_match[i].str() != "" ; ++i)
                //{
                //    friends.push_back(record_match[i].str());
                //}
                std::size_t i = 4;
                while (record_match[i].str() != "")
                {
                    friends.push_back(record_match[i++].str());
                }
                records.insert({username, Record{hashed_pwd, salt, friends}});
            }
            else
            {
                LOG("Invalid record in the database :\n\t" + line);
                continue;
            }

//            if(line[0] == '#' || line.empty()) continue; //Comment
//            if(line.size() < 2+1+2+1+4+1+0+1) //username+:+pwd+:+salt+:+friends+:
//            {
//                LOG("WARNING :: Ignoring line " + std::to_string(line_num) + " at " + USER_LIST + " because of small size." );
//                continue;
//            }
//
//            auto first_colon = line.find(':', 0);
//            auto second_colon = line.find(':', first_colon + 1);
//            auto third_colon = line.find(':', second_colon + 1);
//            auto fourth_colon = line.find(':', third_colon + 1);
//
//            if(first_colon == std::string::npos
//               || second_colon == std::string::npos
//                || third_colon == std::string::npos
//                 || first_colon < 2 //size of user name
//                  || second_colon - first_colon - 1 < 2 //size of password
//                   || third_colon - second_colon - 1 < 4 //size of salt
//              )
//            {
//                LOG("WARNING :: Invalid record at " + USER_LIST + " : " + std::to_string(line_num));
//                continue;
//            }
//
//            if (fourth_colon != std::string::npos)
//            {
//                auto previous_separator = third_colon;
//                auto next_separator = line.find(',', previous_separator + 1);
//
//                while (next_separator != std::string::npos)
//                {
//                    friends.push_back(line.substr(previous_separator + 1, next_separator - previous_separator - 1));
//                    previous_separator = next_separator;
//                    next_separator = line.find(',', previous_separator + 1);
//                }
//            }
//
//            username = line.substr(0, first_colon);
//            hashed_pwd = line.substr(first_colon + 1, second_colon - first_colon - 1);
//            salt = line.substr(second_colon + 1, third_colon - second_colon - 1);
        }
        dbFile.close();
    }
}
