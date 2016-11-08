/******************************************************************
 *                                                                *
 *                Prattle/Server/UserDatabse.hpp                  *
 *                                                                *
 *    ========================================================    *
 *                                                                *
 *    Contains the class to handle all database related tasks.    *
 *                                                                *
 *    See Documentation.md on                                     *
 *    https://github.com/TheIllusionistMirage/Prattle for         *
 *    more details.                                               *
 *                                                                *
 ******************************************************************/


#ifndef USERDATABASE_HPP
#define USERDATABASE_HPP

#include <fstream>
#include <map>
#include <iostream>
#include "../include/System.hpp"

namespace prattle
{
    ///////////////////////////////////////////////////////////////////
    //                                                               //
    //  UserDatabase is a complete interface for interacting with    //
    //  members.db to fetch/modify registered records and add new    //
    //  ones.                                                        //
    //                                                               //
    //  Records are stored using the Record structure. The           //
    //  complete records in members.db are stored as a map with      //
    //  the usernames as the keys to the records.                    //
    //                                                               //
    ///////////////////////////////////////////////////////////////////

    // Holds the records partially. Used in conjunction with std::map
    struct Record
    {
        std::string              hashed_pwd ; // Store hashed password
        std::string              salt       ; // Store the password salt
        std::vector<std::string> friends    ; // Store the friends
    };

    class UserDatabase
    {
        public:

            // Returns the database size (no. of regeistered users).
            unsigned int getDbSize();

            // Returns TRUE if the username-password combination
            // provided for logging in matches a corresponding entry
            // in the database. Else returns FALSE.
            bool isValidPassword(const std::string& username ,
                                 const std::string& plain_pwd);

            // Returns all usernames in database.
            const std::vector<std::string> getAllUsernames();

            // Returns all the friends of a specified user
            const std::vector<std::string>&
            getFriends(const std::string& username);

            // Add a new user to the database. The function returns
            // TRUE if operation was successful. Else FALSE is
            // returned and errors, if any, are logged
            bool addNewUser(const std::string& username ,
                            const std::string& plain_pwd);

            // Inserts a specified user (2nd argument) as the
            // friend of an existing user (1st argument)
            bool addNewFriend(const std::string& username ,
                              const std::string& friendname);

            // Returns TRUE if the user entry exists in the database
            bool isUserRegistered(const std::string& username);

            // Returns all users who usernames contain
            // the search substring (2nd argument)
            std::vector<std::string>
            getMatchingUsers(const std::string& sender ,
                             const std::string& str);

            // Delete an existing user's record from the database
            bool removeUser(const std::string& username);

            UserDatabase();

        private:

            // Private helper called by addNewFriend() to make
            // changes to an existing user entry in the database.
            bool updateRecordOnFile(const std::string& username);

            // Parse the file and read the records.
            void parseFile();

            // The database file (i.e., members.db).
            std::fstream dbFile;

            // The map of records built after reading the database.
            std::map<std::string, Record> records;

            /* Debugging functions */
            void printAllRecords();
    };
}

#endif // USERDATABASE_HPP
