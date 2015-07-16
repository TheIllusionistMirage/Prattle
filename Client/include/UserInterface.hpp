/**

    Prattle/Client/UserInterface.hpp
    =====================

    Manages the nice and visually appeasing UI of Prattle,
    with the help of TGUI, which is built upon SFML.

*/


#ifndef USERINTERFACE_HPP
#define USERINTERFACE_HPP

#include <iostream>
#include <vector>
#include <string>
#include "../include/Config.hpp"

namespace prattle
{
    class UserInterface
    {
            public:

                enum State
                {
                    Login,
                    Signup,
                    Connecting,
                    Chatting,
                    Exit
                };

                enum UIEvent
                {
                    StateChanged,
                    UserLogin,
                    UserSignup,
                    MessageSent,
                    Search,
                    AddFriend,
                    None,
                    Closed,
                    Disconnect
                };

            public:
                virtual void reset() = 0;
                virtual void setState(State s) = 0;
                virtual State getState() = 0;
                virtual UIEvent update() = 0;
                virtual void draw() = 0;

                virtual void alert(const std::string& message) = 0;

                virtual std::string getInputText() = 0;
                virtual std::string getUsername() = 0;
                virtual std::string getPassword() = 0;

                virtual void clearChat() = 0;
                virtual std::string getChat(const std::string& user) = 0;
                virtual void addToChatArea(const std::string& text) = 0;

                virtual bool isStringWhitespace(const std::string& str) = 0;

                virtual void fillFriendList(const std::vector<std::string>& friends) = 0;

//            protected:
//                State m_state;
    };
}

#endif // USERINTERFACE_HPP
