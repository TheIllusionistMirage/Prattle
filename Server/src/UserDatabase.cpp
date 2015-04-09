/**

    Prattle/Server/UserDatabase.hpp
    ===============================

    Contains implementations of class UserDatabase declared in Prattle/Server/UserDatabase.hpp

*/

#include "../include/UserDatabase.hpp"

namespace prattle
{
    UserDatabase::UserDatabase() :
        dbFile(USER_LIST, std::ios::in | std::ios::out),
        records()
    {
        parseFile();
    }

    void UserDatabase::resetDatabase()
    {
        dbFile.seekg(0);
        dbFile.seekp(0);
        dbFile.clear();
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

        Record newRecord{"", generate_salt()};
        newRecord.hashed_pwd = pwd_hash(plain_pwd, newRecord.salt);
        records.insert({username, newRecord});

        dbFile.open(USER_LIST, std::ios::out | std::ios::ate);
        if(!(dbFile << username << ':' << newRecord.hashed_pwd << ':' << newRecord.salt << ':' << std::endl))
        {
            LOG("FATAL ERROR :: Error in writing a new recored to database " + USER_LIST + "!");
            throw std::runtime_error("Error while writing new record to " + USER_LIST);
        }

        parseFile();
        return true;
    }

    bool UserDatabase::addNewFriend(const std::string& username, const std::string& friendname)
    {
        if (isUserRegistered(username) && isUserRegistered(friendname))
        {
            auto record1 = records.find(username);
            auto record2 = records.find(friendname);

            record1->second.friends.push_back(friendname);
            record2->second.friends.push_back(username);

            updateRecordOnFile(username);
            updateRecordOnFile(friendname);

            return true;
        }
        else
        {
            LOG("ERROR :: " + username + " is not registered!");
            return false;
        }

        return false;
    }

    bool UserDatabase::updateRecordOnFile(const std::string& username)
    {
        if(dbFile.bad())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        std::string line;
        dbFile.clear();
        dbFile.seekg(0, std::ios::end);
        std::streamsize fsize = dbFile.tellg();
        dbFile.seekg(0, std::ios::beg);
        std::size_t recordPostion; //the point where the updated record begins

        for(std::getline(dbFile,line); !dbFile.eof(); std::getline(dbFile, line))
        {
            auto first_colon = line.find(':', 0);
            if(first_colon != std::string::npos && line.substr(0, first_colon) == username)
            {
                break;
            }
            recordPostion = dbFile.tellg();
        }

        std::string temp; //stores the rest of the file
        temp.resize(fsize-recordPostion);
        dbFile.read(&temp[0], temp.size());

        dbFile.open(USER_LIST, std::ios::out);
        if(dbFile.bad())
        {
            LOG("ERROR writing to file.");
            std::cerr << "ERROR writing to file\n";
            return false;
        }
        dbFile.seekp(recordPostion, std::ios::beg);
        auto& record = records[username];
        dbFile << username << ':' << record.hashed_pwd << ':' << record.salt << ':';
        if (record.friends.size() > 0)
        {
            for (auto &itr : record.friends)
            {
                dbFile << itr << ',';
            }

            dbFile << ':' << std::endl;
        }
        dbFile.write(&temp[0], temp.size());
        return true;
    }

    void UserDatabase::parseFile()
    {
        dbFile.open(USER_LIST, std::ios::in);
        resetDatabase();

        if(dbFile.bad())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        //TO DO : Improve the file parsing , making it robust
        std::string line;

        dbFile.seekg(std::ios::beg);
        dbFile.seekp(std::ios::beg);

        records.clear();

        std::getline(dbFile,line);

        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile, line) , ++line_num)
        {
            std::string username, hashed_pwd, salt;
            std::vector<std::string> friends;

            if(line[0] == '#' || line.size() < 2+1+2+1+0+1) //username+:+pwd+:+salt+:
                continue;

            auto first_colon = line.find(':', 0);
            auto second_colon = line.find(':', first_colon + 1);
            auto third_colon = line.find(':', second_colon + 1);
            auto fourth_colon = line.find(':', third_colon + 1);

            if(first_colon == std::string::npos
               || second_colon == std::string::npos
                || third_colon == std::string::npos
                 || first_colon < 2 //size of user name
                  || second_colon - first_colon - 1 < 2 //size of password
              )
            {
                LOG("WARNING :: Invalid record at " + USER_LIST + " : " + std::to_string(line_num));
                continue;
            }

            if (fourth_colon != std::string::npos)
            {
                auto previous_separator = third_colon;
                auto next_separator = line.find(',', previous_separator + 1);

                while (next_separator != std::string::npos)
                {
                    friends.push_back(line.substr(previous_separator + 1, next_separator - previous_separator - 1));
                    previous_separator = next_separator;
                    next_separator = line.find(',', previous_separator + 1);
                }
            }

            username = line.substr(0, first_colon);
            hashed_pwd = line.substr(first_colon + 1, second_colon - first_colon - 1);
            salt = line.substr(second_colon + 1, third_colon - second_colon - 1);
            records.insert({username, Record{hashed_pwd, salt, friends}});
        }
    }
}
