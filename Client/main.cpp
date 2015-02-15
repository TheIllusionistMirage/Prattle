#include <iostream>
#include "../include/Client.hpp"

int main()
{
    std::cout << "Client test" << std::endl;

    try
    {
        prattle::Client client;
        client.run();
    }

    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
