#include <iostream>
#include "include/Server.hpp"
#include "include/ErrorLog.hpp"

int main()
{
    try
    {
        prattle::Server server;
        server.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
