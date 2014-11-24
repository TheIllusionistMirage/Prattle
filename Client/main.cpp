#include <exception>
#include <cctype>
#include "Client.hpp"

int main()
{
    try
    {
        chat::Client client;
        client.run();
        /*client.loginPrompt();

        std::cout << "[ Press ENTER to update chat console ]" << std::endl;

        while (client.isLoggedIn())
        {
            std::string message;

            client.receive();

            std::cout << "Me : ";
            std::getline(std::cin, message, '\n');

            sf::Packet msgPacket;

            msgPacket << client.getUserName() << client.getFriendName() << message;

            if (client.send(msgPacket) == sf::Socket::Error)
                std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Error in sending message! Please try again" << std::endl;

            sf::sleep(sf::milliseconds(1));
        }

        client.logout();*/

        return 0;
    }

    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
