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
        if (!dbFile.good())
        {
            LOG("FATAL ERROR :: Unable to open the user database" + USER_LIST);
            throw std::runtime_error("ERROR :: Unable to open the user database " + USER_LIST + " ! Exiting application.");
        }

        parse_file();
        dbFile.close();
    }

    void UserDatabase::resetDatabase()
    {
        dbFile.seekg(0);
        dbFile.seekp(0);
        dbFile.clear();
    }

    bool UserDatabase::isDatabaseOpen()
    {
        return dbFile.good() && dbFile.is_open();
    }

    bool UserDatabase::isUserRegistered(const std::string& username)
    {
        return records.find(username) != records.end();
    }

    const Record& UserDatabase::getRecord(const std::string& username)
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);
        auto itr = records.find(username);

        if (itr == records.end())
        {
            LOG("FATAL ERROR :: Invalid record exists in database!");
            throw std::runtime_error("FATAL ERROR :: Invalid record exists in database!");
        }

        dbFile.close();
        return itr->second;
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
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);

        if(!isDatabaseOpen())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        if(isUserRegistered(username))
        {
            return false;
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

        parse_file();
        dbFile.close();

        return true;
    }

    bool UserDatabase::addNewFriend(const std::string& username, const std::string& friendname)
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);

        if(!isDatabaseOpen())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        if (isUserRegistered(username) && isUserRegistered(friendname))
        {
            // Code needs some critics. Instead of adding these users to
            // each other, this could be done from Server::receive() too.

            std::vector<std::string> f_temp = getRecord(username).friends;
            f_temp.push_back(friendname);
            std::vector<std::string> u_temp = getRecord(friendname).friends;
            u_temp.push_back(username);

            Record r_temp1 = {getRecord(username).hashed_pwd,
                         getRecord(username).salt,
                          f_temp};

            Record r_temp2 = {getRecord(friendname).hashed_pwd,
                         getRecord(friendname).salt,
                          u_temp};

            updateRecord(username, r_temp1);
            updateRecord(friendname, r_temp2);
            parse_file();
            return true;
        }

        else
        {
            LOG("ERROR :: " + username + " is not registered!");
            dbFile.close();
            return false;
        }

        dbFile.close();
        return false;
    }

    bool UserDatabase::updateRecord(const std::string& username, const Record& record)
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);
        resetDatabase();

        if(!isDatabaseOpen())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            return false;
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        std::ofstream tempDbFile("temp_db.dat");
        std::string line;
        std::string name, hashed_pwd, salt;
        std::vector<std::string> friends;
        dbFile.seekg(std::ios::beg);
        dbFile.seekp(std::ios::beg);
        std::getline(dbFile,line);

        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile, line) , ++line_num)
        {
            auto first_colon = line.find(':', 0);

            if(first_colon != std::string::npos && line.substr(0, first_colon) == username)
            {
                tempDbFile << username << ':' << record.hashed_pwd << ':' << record.salt << ':';

                if (record.friends.size() > 0)
                {
                    for (auto &itr : record.friends)
                    {
                        tempDbFile << itr << ',';
                    }

                    tempDbFile << ':' << std::endl;
                }
            }

            else
            {
                tempDbFile << line << std::endl;
            }
        }

        tempDbFile.close();
        dbFile.close();
        std::remove(USER_LIST.c_str());
        std::rename("temp_db.dat", USER_LIST.c_str());
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);
        parse_file();
        dbFile.close();

        return true;
    }

    bool UserDatabase::reloadAllRecords()
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);

        if (!dbFile.good())
        {
            LOG("FATAL ERROR :: Unable to open the user database" + USER_LIST);
            return false;
            throw std::runtime_error("ERROR :: Unable to open the user database " + USER_LIST + " ! Exiting application.");
        }

        parse_file();
        dbFile.close();

        return true;
    }

    void UserDatabase::parse_file()
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);
        resetDatabase();

        if(!isDatabaseOpen())
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

        dbFile.close();
    }
}
