#include "Client.hpp"

int main()
{
    chat::Client client;
    //client.login();
    client.loginPrompt();
    client.login();

    while (client.isLoggedIn())
    {
        std::cout << "Me : ";
		std::string message;
		//std::cin >> message;
		std::getline(std::cin, message);

		sf::Packet msgPacket;
		msgPacket << message;

		if (client.send(msgPacket) == sf::Socket::Error)
			std::cout << "> Error in sending message! Please try again" << std::endl;

		sf::sleep(sf::milliseconds(1));
    }

    client.logout();

    return 0;
}
