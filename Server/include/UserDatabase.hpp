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
#include "../include/System.hpp"

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

    // Class UserDatabase which handles all tasks related
    // to the user database (i.e., members.db)
    class UserDatabase
    {
        public:

            UserDatabase();

            void resetDatabase();                               // Reset the database by setting read
                                                                // and write pointer at the beginning

            bool isDatabaseOpen();                              // Returns true if members.db is open and
                                                                // is okay for read/write operations.

            bool isUserRegistered(const std::string& username);     // Returns true if 'username' is registered.

            const Record& getRecord(const std::string& username); // Returns the database record for the user 'username'.

            bool isValidPassword(const std::string& username,
                                 const std::string& plain_pwd); // Returns true if the username/password
                                                                // combination is matching an entry in the user database.
            bool addNewUser(const std::string &username,
                            const std::string& plain_pwd);      // Returns true if 'username' was added to the database.

            bool addNewFriend(const std::string& username,
                              const std::string& friendname);   // Add a friend's name for the person 'username'.
                                                                // Returns if the addition was successful.
            bool updateRecord(const std::string& username,
                              const Record& record);            // Updates the record for the user 'username'.

            bool reloadAllRecords();                            // Reload all records from 'members.db'

        private:

            void                          parse_file();         // Parse the file and read the records.
            std::fstream                  dbFile;               // The database file (i.e., members.db).
            std::map<std::string, Record> records;              // The map of records built after reading the database.
    };
}

#endif // USERDATABASE_HPP
