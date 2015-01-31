/**

    Prattle/Server/UserDatabse.hpp
    ===============================

    Contains the class to handle all database related tasks.

    Records are stored as:

        # MEMBERS.DAT
        # Lines beginning with '#' are ignored

        USERNAME_1:HASHED_PASSWORD:SALT:FRIEND-1,FRIEND-2,...,FRIEND-N,:
        USERNAME_2:HASHED_PASSWORD:SALT:FRIEND-1,FRIEND-2,...,FRIEND-N,:
        USERNAME_3:HASHED_PASSWORD:SALT:FRIEND-1,FRIEND-2,...,FRIEND-N,:

*/

#ifndef USERDATABASE_HPP
#define USERDATABASE_HPP

#include <fstream>
#include <map>
#include <iostream>
#include "System.hpp"

namespace prattle
{
    // This the format in which the records minus the username are stored
    // The username is used as the key for the map which stores these records

    struct Record
    {
        std::string hashed_pwd;
        std::string salt;
        std::vector<std::string> friends;
    };

    class UserDatabase
    {
        public:

            UserDatabase();

            void resetDatabase();
            bool isDatabaseOpen();
            bool isUserRegistered(const std::string& name);     // Returns true if 'name' is registered.
            const Record& getRecord(const std::string& username) const;
            bool isValidPassword(const std::string& name,
                                 const std::string& plain_pwd); // Returns true if the username/password
                                                                // combination is matching an entry in the user database.
            bool addNewUser(const std::string &name,
                            const std::string& plain_pwd);      // Returns true if 'name' was added to the database.
            bool addNewFriend(const std::string& username,      // Add a friend's name for the person 'username'
                              const std::string& friendname);
            void updateRecord(const std::string& username,
                              const Record& record);
            void deleteRecord(const std::string& username,
                              const Record& record);

        private:

            void                          parse_file();         // Parse the file and read the records.
            std::fstream                  dbFile;               // The database file (called members.dat).
            std::map<std::string, Record> records;              // Records are treated as USERNAME : HASHED_PASSWORD:SALT:
    };
}

#endif // USERDATABASE_HPP
