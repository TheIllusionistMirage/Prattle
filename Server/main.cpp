#include <iostream>
#include "include/Server.hpp"

int main()
{
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
