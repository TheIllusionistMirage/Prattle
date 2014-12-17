#include "Client.hpp"

namespace prattle
{
    Client::Client() : m_networkManager{},
                       m_ui{}
    {
        m_windowPtr = &m_ui.getRenderWindow();
        m_guiPtr = &m_ui.getGui();
    }

    void Client::reset()
    {
        m_networkManager.reset();
        m_ui.reset();
    }

    void Client::run()
    {
        while (m_windowPtr->isOpen())
        {
            sf::Event event;
            while (m_windowPtr->pollEvent(event))
            {

            }

            m_windowPtr->clear(sf::Color::Black);
            //m_windowPtr
        }
    }
}
