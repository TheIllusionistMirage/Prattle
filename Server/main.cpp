#include <iostream>
#include "../include/Server.hpp"

int main()
{
    //prattle::Server s;
    //prattle::UserDatabase usrdb;
    //usrdb.addNewFriend("trex", "abcd");
    //usrdb.updateRecord("trex", prattle::Record{"876a87f5eeac656ca69b38d9863bea5e03854f38b47348073724629b64a6cf0b", "DYbiG", std::vector<std::string>{"john", "robert"}});
    std::cout << "Server test" << std::endl;
    //sf::Packet p;
    //p << LOGIN_SUCCESS << "foo bar";
    //s.send(p);
    /*prattle::ErrorLog::Logger()->log("testing msg stdout 1", __FILE__, __LINE__);
    prattle::ErrorLog::Logger()->log("testing msg file 1", __FILE__, __LINE__, "stdout");
    prattle::ErrorLog::Logger()->log("testing msg stdout 2", __FILE__, __LINE__);
    prattle::ErrorLog::Logger()->log("testing msg file 2", __FILE__, __LINE__);
    prattle::ErrorLog::Logger()->log("testing msg stdout 3", __FILE__, __LINE__);
    prattle::ErrorLog::Logger()->log("testing msg stdout 4", __FILE__, __LINE__);
    prattle::ErrorLog::Logger()->log("testing msg file 3", __FILE__, __LINE__);*/
    //prattle::LOG("testing main()");
    //s.shutdown();

    /*try
    {
        prattle::Server server;

        while (server.isRunning())
        {
            if (server.wait())
            {
                if (server.newConnectionRequest())
                {
                    server.addNewClient();
                }
                else
                {
                    server.receive();
                }
            }

            sf::sleep(sf::milliseconds(1));
        }

        server.shutdown();
    }

    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }*/

    try
    {
        prattle::Server server;
        server.run();

    }

    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
