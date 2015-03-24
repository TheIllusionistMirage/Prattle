/**

    Prattle/Client/Client.hpp
    =========================

    The client side application for chatting. Client contains instances of
    Prattle/Client/NetworkManager and Prattle/Client/GUI each.

*/

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/NetworkManager.hpp"
#include "../include/UI.hpp"

namespace prattle
{
    class Client
    {
        public:

            //Client(std::string s_ip = "127.0.0.1", int s_port = 19999);
            Client();

            //void insertNotification(const std::string& username);   // add a notification as '[n]' where n = no. of unread messages from user 'username'

            void addFriend();                                   // Add the person who's name was received after a database search.

            bool searchUsername(const std::string& username);   // Returns true if 'username' was found in the user database.

            bool checkIfWhitespace(const std::string& message); // Returns true if 'message' is a whitespace string.

            bool isLoggedIn();                                  // Returns true if the Client is logged in , i.e.,
                                                                // m_networkManager is connected to the Server.

            std::string const& getUserName() const;             // Returns the username of the Client.

            void logout();                                      // Disconnect from the Server.

            bool login();                                       // Connect to the Server through a login process.

            bool signup();                                      // Create an account. Before Logging in, the Client
                                                                // needs an account.

            void reset();                                       // Reset the Client application to a default state.

            void run();                                         // Runs the Client and has the main control loop of the program.

        protected:
            void parseConfigFile();

        private:

            //std::map<std::string, std::string> m_chatHistory;

            bool m_loginStatus;                                 // Is set to true if m_networkManager is connected to the Server.
            std::string m_username;                             // Stores the username of the Client.
            std::string m_password;                             // Stores the password of the Client.
            std::vector<std::string> m_friends;                 // Stores the usernames of the Client's friends.
                                                                // NOTE : Currently not under use.
            std::vector<std::string> m_friendsOnline;           // Stores the usernames of the Client's friends currently online.
                                                                // NOTE : Currently not under use.
            Status m_onlineStatus;                              // The online status of the Client.
            NetworkManager m_networkManager;                    // Instance of Class NetworkManager to handle
                                                                // communication with the Server.
            UI m_ui;                                            // Instance of Class UI to handle the GUI of the Client.
            std::string m_server_ip;                            // Server ip
            int m_server_port;                                  // Server port
            std::ifstream m_configFile;                         // Stores basic server configuration
    };
}

#endif // CLIENT_HPP
