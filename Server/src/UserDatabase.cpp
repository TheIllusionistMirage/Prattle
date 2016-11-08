/******************************************************************
 *                                                                *
 *                Prattle/Server/UserDatabse.cpp                  *
 *                                                                *
 *    ========================================================    *
 *                                                                *
 *    Contains implementation of class UserDatabase declared      *
 *    in UserDatabase.hpp.                                        *
 *                                                                *
 *    See Documentation.md on                                     *
 *    https://github.com/TheIllusionistMirage/Prattle for         *
 *    more details.                                               *
 *                                                                *
 ******************************************************************/


#include "../include/UserDatabase.hpp"
#include <regex>

namespace prattle
{
    unsigned int UserDatabase::getDbSize()
    {
        return records.size();
    }

    bool UserDatabase::isValidPassword(const std::string& username ,
                                       const std::string& plain_pwd)
    {
        auto res = records.find(username);

        if(res == records.end())
            return false;

        return res->second.hashed_pwd == pwd_hash(plain_pwd ,
                                                  res->second.salt);
    }

    const std::vector<std::string> UserDatabase::getAllUsernames()
    {
        std::vector<std::string> usernames;

        for (auto& i : records)
            usernames.push_back(i.first);

        return usernames;
    }

    const std::vector<std::string>&
    UserDatabase::getFriends(const std::string& username)
    {
        auto itr = records.find(username);

        if (itr == records.end())
        {
            WRN_LOG("Invalid username queried at function getFriends()");
            throw std::runtime_error("Invalid username queried at function getFriends()");
        }

        return itr->second.friends;
    }

    bool UserDatabase::addNewUser(const std::string &username ,
                                  const std::string& plain_pwd)
    {
        if(isUserRegistered(username))
        {
            DBG_LOG("User already registered using that name!");
            return false;
        }

        // If non-registered username...

        dbFile.open(USER_LIST, std::ios::in | std::ios::out);

        if(dbFile.bad())
        {
            ERR_LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " +
                                     USER_LIST + "!");
        }

        // Create a new record to hold details temporarily.
        Record newRecord{"", generate_salt(), {}};
        newRecord.hashed_pwd = pwd_hash(plain_pwd, newRecord.salt);
        records.insert({username, newRecord});

        // Go to the end of the database.
        dbFile.seekp(0, std::ios::end);

        if(!(dbFile << username
                    << ':'
                    << newRecord.hashed_pwd
                    << ':'
                    << newRecord.salt
                    << "::"
                    << std::endl))
        {
            ERR_LOG("FATAL ERROR :: Error in writing a new recored to database " + USER_LIST + "!");
            throw std::runtime_error("Error while writing new record to " + USER_LIST);
        }

        // Successfully updated database
        dbFile.close();
        return true;
    }

    bool UserDatabase::addNewFriend(const std::string& username ,
                                    const std::string& friendname)
    {
        // Do a safety check first to prevent undefined behaviour
        if (isUserRegistered(username) && isUserRegistered(friendname))
        {
            auto& user_rec = records[username];
            auto& friend_rec = records[friendname];

            // If `username` and `friendname` are
            // already friends, nothing left to do
            for (auto& i : user_rec.friends)
                if (i == friendname)
                    return false;

            for (auto& j : friend_rec.friends)
                if (j == username)
                    return false;


            // Update server's map
            user_rec.friends.push_back(friendname);
            friend_rec.friends.push_back(username);

            // Update the database
            updateRecordOnFile(username);
            updateRecordOnFile(friendname);

            return true;
        }

        else
        {
            DBG_LOG("ERROR :: Either " + username + " or " +
                    friendname + " is not registered!");
            return false;
        }

        return false;
    }

    bool UserDatabase::isUserRegistered(const std::string& username)
    {
        return records.find(username) != records.end();
    }

    std::vector<std::string>
    UserDatabase::getMatchingUsers(const std::string& sender ,
                                   const std::string& str)
    {
        std::vector<std::string> matches;

        for (auto& i : records)
        {
            if (i.first.find(str) != std::string::npos)
                if (i.first != sender)
                    matches.push_back(i.first);
        }

        return matches;
    }

    bool UserDatabase::removeUser(const std::string& username)
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);

        if(dbFile.bad())
        {
            ERR_LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
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

    UserDatabase::UserDatabase()
    {
        parseFile();
    }

    bool UserDatabase::updateRecordOnFile(const std::string& username)
    {
        dbFile.open(USER_LIST, std::ios::in | std::ios::out);

        if(dbFile.bad())
        {
            ERR_LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
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
        const std::regex record_pattern(
                "([\\w.]{3,16}):([0-9a-f]{64}):([[:alnum:]]+):((?:[\\w.]{3,16},)*):"),
                friend_pattern("([\\w.]{3,16}),"),
                comment_pattern("\\s*#.*");

        dbFile.open(USER_LIST, std::ios_base::in);
        if(dbFile.bad())
        {
            ERR_LOG("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
            throw std::runtime_error("FATAL ERROR :: Error in parsing " + USER_LIST + "!");
        }
        records.clear();
        std::string line;
        std::getline(dbFile,line);
        for(unsigned int line_num = 1; !dbFile.eof(); std::getline(dbFile, line) , ++line_num)
        {
            std::smatch record_match;
            if(line.empty() || std::regex_match(line, comment_pattern))
                continue;
            else if(std::regex_match(line, record_match, record_pattern))
            {
                std::string username {record_match[1].str()},
                             hashed_pwd {record_match[2].str()},
                             salt {record_match[3].str()},
                             friendsAll {record_match[4].str()};

                std::vector<std::string> friends;
                std::smatch friend_match;
                while (std::regex_search(friendsAll, friend_match, friend_pattern))
                {
                    friends.push_back(friend_match[1].str());
                    friendsAll = friend_match.suffix().str();
                }

                records.insert({username, Record{hashed_pwd, salt, friends}});
            }
            else
            {
                WRN_LOG("Invalid record in the database.\n\tLine: " + line);
                continue;
            }
        }
        dbFile.close();
    }

    /* Debugging functions */

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
}
