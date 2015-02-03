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
    }

    bool UserDatabase::isUserRegistered(const std::string& name)
    {
        return records.find(name) != records.end();
    }

    const Record& UserDatabase::getRecord(const std::string& username) const
    {
        auto itr = records.find(username);
        if (itr == records.end())
        {
            LOG("FATAL ERROR :: Invalid record exists in database!");
            throw std::runtime_error("FATAL ERROR :: Invalid record exists in database!");
        }
        //std::cout << itr->second.friends.size() << std::endl;
        return itr->second;
    }

    bool UserDatabase::isValidPassword(const std::string&name,const std::string& plain_pwd)
    {
        auto res = records.find(name);

        if(res == records.end())
        {
            return false;
        }

        return res->second.hashed_pwd == pwd_hash(plain_pwd,res->second.salt);
    }

    bool UserDatabase::addNewUser(const std::string &name,const std::string& plain_pwd)
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);

        if(!isDatabaseOpen())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        if(isUserRegistered(name))
        {
            return false;
        }

        Record newRecord{"",generate_salt()};
        newRecord.hashed_pwd = pwd_hash(plain_pwd,newRecord.salt);
        records.insert({name,newRecord});
        dbFile.seekp(0,std::ios::end);

        if(!(dbFile << name << ':' << newRecord.hashed_pwd << ':' << newRecord.salt << ':' << std::endl))
        {
            LOG("FATAL ERROR :: Error in writing a new recored to database " + USER_LIST + "!");
            throw std::runtime_error("Error while writing new record to " + USER_LIST);
        }

        dbFile.close();

        return true;
    }

    bool UserDatabase::addNewFriend(const std::string& username, const std::string& friendname)
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);

        resetDatabase();

        if(!isDatabaseOpen())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        if (isUserRegistered(username))
        {
            std::vector<std::string> f_temp = getRecord(username).friends;
            f_temp.push_back(friendname);

            Record r_temp = {getRecord(username).hashed_pwd,
                         getRecord(username).salt,
                          f_temp};

            updateRecord(username, r_temp);
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

    void UserDatabase::updateRecord(const std::string& username, const Record& record)
    {
        resetDatabase();

        if(!isDatabaseOpen())
        {
            LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }

        //std::ofstream tempDbFile("temp_db.dat");
        std::vector<std::string> tempDBRecordPlaceholder;

        //
        std::string line;
        std::string name, hashed_pwd, salt;
        std::vector<std::string> friends;

        dbFile.seekg(std::ios::beg);
        dbFile.seekp(std::ios::beg);

        std::getline(dbFile,line);

        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile, line) , ++line_num)
        {
            //if(line[0] == '#' || line.size() < 2+1+2+1+0+1) //username+:+pwd+:+salt+:
            //    continue;

            auto first_colon = line.find(':', 0);

            if(first_colon != std::string::npos && line.substr(0, first_colon) == username)
            {
                //std::cout << line << std::endl;
                //tempDbFile << username << ':' << record.hashed_pwd << ':' << record.salt << ':';
                tempDBRecordPlaceholder.push_back(username + ':' + record.hashed_pwd + ':' + record.salt + ':');

                if (record.friends.size() > 0)
                {
                    for (auto &itr : record.friends)
                    {
                        //tempDbFile << itr << ',';
                        tempDBRecordPlaceholder.push_back(itr + ",");
                    }
                    //tempDbFile << ':' << std::endl;
                    tempDBRecordPlaceholder.push_back(":\n");
                }
            }

            else
            {
                //tempDbFile << line << std::endl;
                tempDBRecordPlaceholder.push_back(line + '\n');
            }
        }
        //

        //tempDbFile.close();
        dbFile.close();
        //std::remove(USER_LIST.c_str());
        //std::rename("temp_db.dat", USER_LIST.c_str());
        //dbFile.open(USER_LIST, std::ios::in | std::ios::out);
        dbFile.open(USER_LIST, std::ios::in | std::ios::out | std::ios::trunc);
        for (auto& itr : tempDBRecordPlaceholder)
        {
            //if (itr != tempDBRecordPlaceholder.end())
            {
                dbFile << itr;
            }
        }
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
        std::string username, hashed_pwd, salt;
        std::vector<std::string> friends;

        dbFile.seekg(std::ios::beg);
        dbFile.seekp(std::ios::beg);

        std::getline(dbFile,line);

        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile, line) , ++line_num)
        {
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
                //std::cerr << "Invalid record at " << USER_LIST << ":" << line_num << "Error raised in " << __FILE__ << ":" << __LINE__ << std::endl;
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
                    //std::cout << line.substr(previous_separator + 1, next_separator - previous_separator - 1) << std::endl;
                    previous_separator = next_separator;
                    next_separator = line.find(',', previous_separator + 1);
                }
            }

            username = line.substr(0, first_colon);
            hashed_pwd = line.substr(first_colon + 1, second_colon - first_colon - 1);
            salt = line.substr(second_colon + 1, third_colon - second_colon - 1);
            records.insert({username, Record{hashed_pwd, salt, friends}});

            //std::cout << line.substr(third_colon + 1, fourth_colon - third_colon - 1) << std::endl;
        }
        dbFile.close();
    }

    bool UserDatabase::isDatabaseOpen()
    {
        return dbFile.good() && dbFile.is_open();
    }

    void UserDatabase::resetDatabase()
    {
        dbFile.seekg(0);
        dbFile.seekp(0);
        dbFile.clear();
    }
}
