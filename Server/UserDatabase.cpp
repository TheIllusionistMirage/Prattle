#include "UserDatabase.h"

namespace chat
{
    UserDatabase::UserDatabase() :
        dbFile(chat::USER_LIST),
        records()
    {
        if (!dbFile.good())
        {
            throw std::runtime_error("ERROR :: Unable to open the user database " + USER_LIST + " ! Exiting application.");
        }
        parse_file();
    }

    bool UserDatabase::isUserRegistered(const std::string& name)
    {
        return records.find(name) != records.end();
    }

    bool UserDatabase::isValidPassword(const std::string&name,const std::string& plain_pwd)
    {
        auto res = records.find(name);
        if(res == records.end())
        {
            return false;
        }
        return res->second == pwd_hash(plain_pwd);
    }

    bool UserDatabase::addNewUser(const std::string &name,const std::string& plain_pwd)
    {
        if(!dbFile.good())
        {
            throw std::runtime_error("The \'dbFile\' is in bad condition , adding new user failed.");
        }
        auto res = records.find(name);
        if(res == records.end())
        {
            return false;
        }
        records.insert(res,std::make_pair(name,pwd_hash(plain_pwd)));
        dbFile.seekp(std::ios::end);
        if(!(dbFile << name << ':' << pwd_hash(plain_pwd) << ':'))
        {
            throw std::runtime_error("Error while writing new record to " + chat::USER_LIST);
        }
        return true;
    }

    void UserDatabase::parse_file()
    {
        if(!dbFile.good())
        {
            throw std::runtime_error("The \'dbFile\' is in bad condition , parsing failed.");
        }
        //TO DO : Improve the file parsing , making it robust
        std::string line;
        std::string username,hashed_pwd;
        std::getline(dbFile,line);
        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile,line) , ++line_num)
        {
            if(line[0] == '#' || line.size() < 2+1+2+1)
                continue;
            auto first_colon = line.find(':',0);
            auto second_colon = line.find(':',first_colon+1);
            username = line.substr(0,first_colon-0);
            if(username.size() < 2)
            {
                std::cerr << "Invalid record at " << chat::USER_LIST << ":" << line_num << "Error raised in " << __FILE__ << ":" << __LINE__ << std::endl;
                continue;
            }
            hashed_pwd = line.substr(first_colon+1,second_colon-first_colon-1);
            if(hashed_pwd.size() < 2)
            {
                std::cerr << "Invalid record at " << chat::USER_LIST << ":" << line_num << "Error raised in " << __FILE__ << ":" << __LINE__ << std::endl;
                continue;
            }
            records.insert(std::make_pair(username,hashed_pwd));
        }
        dbFile.clear();
    }
}
