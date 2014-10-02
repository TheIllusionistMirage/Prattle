#include "Server.hpp"
#include <exception>

int main()
{
    try
    {
        chat::Server server;
        server.getRecords();

        while (server.isRunning())
        {
            if (server.wait())
            {
                //std::cout << "ADASD" << std::endl;
                if (server.isReady())
                {
                    //std::cout << "ADASD" << std::endl;
                    server.addNewClient();
                }

                else
                {
                    server.receive();
                }

                //std::cout << "ADASD" << std::endl;
            }

            /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                server.shutdown();*/

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
