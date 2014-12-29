#include "NetworkManager.hpp"

namespace prattle
{
    NetworkManager::NetworkManager() : m_clientSocket{}
    {

    }

    void NetworkManager::reset()
    {

        m_clientSocket.disconnect();
        m_clientSocket.setBlocking(true);
    }

    bool NetworkManager::connectToServer(const std::string serverIP, const int port)
    {
        sf::Socket::Status status = m_clientSocket.connect(serverIP, port);

        if (status == sf::Socket::Done)
            return true;

        return false;
    }

    void NetworkManager::setSocketBlocking(bool blocking)
    {
        m_clientSocket.setBlocking(blocking);
    }

    bool NetworkManager::send(const std::string& sender, const std::string& receiver, const std::string& message)
    {
        sf::Packet packet;
        packet << sender << receiver << message;

        sf::Socket::Status status = m_clientSocket.send(packet);

        if (status == sf::Socket::Status::Done)
            return true;

        return false;
    }

    /*
        TO DO: Make receiving packets more efficient and refraining from using the '>>' and '<<' operators
    */
    bool NetworkManager::receive(std::string& message)
    {
        sf::Packet packet;
        sf::Socket::Status status = m_clientSocket.receive(packet);

        std::string data;

        if (status == sf::Socket::NotReady)
        {
            std::cout << "X" << std::endl;
            return false;
        }

        else if (status == sf::Socket::Done)
        {
            if (packet >> data)
            {
                message = data;

                std::cout << "X" + data +"X" << std::endl;

                return true;
            }
        }

        else if (status == sf::Socket::Error)
        {
            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Unable to receive data from remote peer!" <<  std::endl;
        }

        return false;
    }

    /*
        TO DO: Make receiving packets more efficient and refraining from using the '>>' and '<<' operators
    */
    bool NetworkManager::receive(std::string& sender, std::string& message)
    {
        sf::Packet packet;
        sf::Socket::Status status = m_clientSocket.receive(packet);

        std::string _sender; // Not sure what identifier to use here
        std::string data;

        if (status == sf::Socket::NotReady)
            return false;

        if (status == sf::Socket::Done)
        {
            if (packet >> _sender >> data)
            {
                sender = _sender;
                message = data;

                return true;
            }
        }

        else if (status == sf::Socket::Error)
        {
            std::cerr << __FILE__ << ':' << __LINE__ << "  ERROR :: Unable to receive data from remote peer!" <<  std::endl;
        }

        return false;
    }
}
