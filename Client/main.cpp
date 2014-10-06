#include <cctype>
#include "Client.hpp"

int main()
{
    try
    {
        chat::Client client;
        client.loginPrompt();

        char choice;
        do
        {
            std::cout << "(R)egister or (L)ogin to start chatting!" << std::endl;
            std::cin >> choice;
            std::cin.ignore();

            switch (tolower(choice))
            {
                case 'r' : client.signup(); break;
                case 'l' : client.login(); break;
                default  : std::cout << "Please enter a valid option!" << std::endl;
            }
        } while (choice != 'r' && choice != 'l');

        std::cout << "Enter a blank message to update ." << std::endl;
        while (client.isLoggedIn())
        {
            client.receive();
            std::cout << "Me : ";
            std::string message;

            std::getline(std::cin, message, '\n');

            sf::Packet msgPacket;
            //msgPacket << client.getUserName() + ":" + client.getFriendName() + ":" + message;
            msgPacket << client.getUserName() << client.getFriendName() << message;

            if (client.send(msgPacket) == sf::Socket::Error)
                std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Error in sending message! Please try again" << std::endl;

            sf::sleep(sf::milliseconds(1));
        }

        client.logout();

        return 0;
    }

    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
