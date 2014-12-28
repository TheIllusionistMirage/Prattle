#include <exception>
#include "Client.hpp"

int main()
{
    try
    {
        prattle::Client client;
        client.run();

        return 0;
    }

    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
