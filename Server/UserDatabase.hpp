#ifndef USERDATABASE_H
#define USERDATABASE_H
#include <fstream>
#include <map>
#include <iostream>
#include "System.hpp"

struct Record
{
    std::string hashed_pwd;
    std::string salt;
};

namespace chat
{
    class UserDatabase
    {
        public:
            UserDatabase();
            bool isUserRegistered(const std::string& name);
            bool isValidPassword(const std::string&name,const std::string& plain_pwd);
            bool addNewUser(const std::string &name,const std::string& plain_pwd);
        private:
            void parse_file();

            std::fstream dbFile;
            //          (User name,Record)
            std::map<std::string,Record> records;
    };
}
#endif // USERDATABASE_H
