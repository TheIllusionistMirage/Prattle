#include "Client.hpp"

int main()
{
    chat::Client client;
    client.loginPrompt();

    /*char choice;
    do
    {
        std::cout << "(R)egister or (L)ogin to start chatting!" << std::endl;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice)
        {
            case 'r' :;
            case 'R' : client.signup(); break;
            case 'l' :;
            case 'L' : client.login(); break;
            default  : std::cout << "Please enter a valid option!" << std::endl;
        }
    } while (choice != 'r' || choice != 'R' || choice != 'l' || choice != 'L');*/

    client.login();
    while (client.isLoggedIn())
    {
        std::cout << "Me : ";
		std::string message;

		std::getline(std::cin, message, '\n');

		sf::Packet msgPacket;
		msgPacket << message;

		if (client.send(msgPacket) == sf::Socket::Error)
			std::cout << "> Error in sending message! Please try again" << std::endl;

		sf::sleep(sf::milliseconds(1));
    }

    client.logout();

    return 0;
}
