#include "Server.hpp"
#include <exception>

int main()
{
    try
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
    }

    return 0;
}
