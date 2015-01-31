#include "UserDatabase.hpp"

namespace prattle
{
    UserDatabase::UserDatabase() :
        dbFile(USER_LIST, std::ios::in | std::ios::out),
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
        return res->second.hashed_pwd == pwd_hash(plain_pwd,res->second.salt);
    }

    bool UserDatabase::addNewUser(const std::string &name,const std::string& plain_pwd)
    {
        if(!dbFile.good())
        {
            throw std::runtime_error("The \'dbFile\' is in bad condition , adding new user failed.");
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
            throw std::runtime_error("Error while writing new record to " + USER_LIST);
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
        std::string username,hashed_pwd,salt;
        std::getline(dbFile,line);
        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile,line) , ++line_num)
        {
            if(line[0] == '#' || line.size() < 2+1+2+1+0+1) //username+:+pwd+:+salt+:
                continue;
            auto first_colon = line.find(':',0);
            auto second_colon = line.find(':',first_colon+1);
            auto third_colon = line.find(':',second_colon+1);
            if(first_colon == std::string::npos || second_colon == std::string::npos || third_colon == std::string::npos
               || first_colon < 2 //size of user name
               || second_colon-first_colon-1 < 2 //size of password
               )
            {
                std::cerr << "Invalid record at " << USER_LIST << ":" << line_num << "Error raised in " << __FILE__ << ":" << __LINE__ << std::endl;
                continue;
            }
            username = line.substr(0,first_colon);
            hashed_pwd = line.substr(first_colon+1,second_colon-first_colon-1);
            salt = line.substr(second_colon+1,third_colon-second_colon-1);
            records.insert({username,Record{hashed_pwd,salt}});
        }
        dbFile.clear();
    }
}
