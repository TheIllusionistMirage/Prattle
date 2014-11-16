#include <exception>
#include <cctype>
#include "Client.hpp"

int main()
{
    try
    {
        chat::Client client;
        client.run();
        //client.loginPrompt();

        //std::cout << "[ Press ENTER to update chat console ]" << std::endl;

        /*sf::RenderWindow window(sf::VideoMode(400, 300), "TGUI window");
        tgui::Gui gui(window);

        gui.setGlobalFont("resources/fonts/DejaVuSans.ttf");

        tgui::Panel::Ptr panel = tgui::Panel::create();
        panel->setPosition(100, 100);
        gui.add(panel);

        tgui::ListBox::Ptr lb = tgui::ListBox::create();
        lb->addItem("abc1", "a");
        lb->addItem("def1", "b");
        lb->addItem("ghi1", "c");

        panel->add(lb);

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();

                gui.handleEvent(event);
            }

            window.clear();

            gui.draw();

            lb->getItemById("a");
            lb->getItemById("b");
            lb->getItemById("c");

            window.display();
        }*/


        /*while (client.isLoggedIn())
        {
            std::string message;

            client.receive();

            std::cout << "Me : ";
            std::getline(std::cin, message, '\n');

            sf::Packet msgPacket;

            msgPacket << client.getUserName() << client.getFriendName() << message;

            if (client.send(msgPacket) == sf::Socket::Error)
                std::cerr << __FILE__ << ':' << __LINE__ << " ERROR :: Error in sending message! Please try again" << std::endl;

            //client.renderScreen();

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
