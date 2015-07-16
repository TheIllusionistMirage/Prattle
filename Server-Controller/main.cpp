#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <SFML/Network.hpp>

using std::cout;
using std::endl;
using std::cerr;

std::vector<std::string> split(const std::string& str)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while(ss >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

int main()
{
    sf::TcpSocket server_sock;
    std::string input, prompt = "Disconnected";
    const std::string prompt_suffix = "> ";
    bool connected = false;

    cout << "Server-Controller by team Prattle" << endl;
    cout << "Use this to control a Prattle Server."
         << "\nType 'help' for displaying supported commands to control the server\n" << endl;

    for(cout << prompt + prompt_suffix; std::getline(std::cin, input);
        cout << prompt + prompt_suffix)
    {
        if(input.empty())
            continue;

        auto tokens = split(input);
        std::string command = tokens[0];
        if(command == "exit")
        {
            break;
        }
        else if(command == "help")
        {
            cout << "Server-Controller by team Prattle\n" << endl
                  << "Use this to control a Prattle Server." << endl
                  << "Available commands : " << endl;
            cout << "Local commands for Server-Controller : " << endl << endl;

            cout << "Command                     Description" << endl;
            cout << "-----------------------------------------------------------------------------" << endl;
            cout << "help                        Print help" << endl;
            cout << "connect <ip> <port>         Connect to Server at IP <ip> and port <port>" << endl;
            cout << "connect <hostname> <port>   Connect to Server at <hostname> e.g. \"localhost\"" << endl;
            cout << "disconnect                  Disconnect from Server" << endl;
            cout << "exit                        Disconnect (if connected) and exit" << endl << endl;

            cout << "Commands sent to the server :" << endl << endl;

            cout << "Command            Description" << endl;
            cout << "-------------------------------------------------------------------------" << endl;
            cout << "shutdown           Shutdown the server" << endl;
            cout << "show_logged_users  Prints the usernames of all logged users" << endl;
            cout << "remove_user <name> Removes user <name> from the DB" << endl;
            cout << "print_stats        Prints stats about the server (uptime, number of users etc.))" << endl;

        }
        else if(command == "connect")
        {
            if(connected)
            {
                cerr << "Invalid Command.\nAlready connected to server at " << server_sock.getRemoteAddress() << endl;
                continue;
            }

            if(tokens.size() < 3)
                cerr << "Not enough arguments for `connect' command." << endl;

            std::string ip = tokens[1];
            unsigned short port = std::stoi(tokens[2]);
            auto status = server_sock.connect(ip, port, sf::seconds(60));
            if(status != sf::Socket::Done)
            {
                cerr << "Error in connecting to server." << endl;
                server_sock.disconnect();
            }
            else
            {
                sf::Packet connect_packet;
                std::string passphrase;
                cout << "Enter the pass-phrase for this server: ";
                std::getline(std::cin, passphrase);
                connect_packet << "controller_attach" << passphrase;
                status = server_sock.send(connect_packet);
                if(status != sf::Socket::Done)
                {
                    cerr << "ERROR while sending packet to server. Connection request unsuccessful." << endl;
                    server_sock.disconnect();
                }
                else
                {
                    sf::Packet connect_reply;
                    status = server_sock.receive(connect_reply);
                    if(status == sf::Socket::Done)
                    {
                        std::string reply;
                        connect_reply >> reply;
                        if(reply == "ack")
                        {
                            prompt = ip;
                            connected = true;
                            cout << "Connected.\n";
                        }
                        else
                        {
                            cout << "Couldn't connect to server. Sever's reply: \n";
                            cout << reply << endl;
                            server_sock.disconnect();
                        }
                    }
                    else
                    {
                        cerr << "ERROR while receiving from server. Status code: " << status << endl;
                        server_sock.disconnect();
                    }
                }
            }
        }
        else if(command == "disconnect")
        {
            server_sock.disconnect();
            connected = false;
            prompt = "Disconnected";
        }
        else
        {
            if(connected)
            {
                sf::Packet packet;
                for(auto& s : tokens)
                {
                    packet << s;
                }
                auto status = server_sock.send(packet);
                if(status != sf::Socket::Done && status != sf::Socket::Disconnected)
                {
                    cerr << "Error sending packet to server." << endl;
                    continue;
                }
                else if (status == sf::Socket::Disconnected)
                {
                    prompt = "Disconnected";
                    cerr << "Unexpectedly disconnected from the server." << endl;
                    server_sock.disconnect();
                    connected = false;
                    continue;
                }

                // Receive a reply
                sf::Packet reply;
                status = server_sock.receive(reply);
                if(status != sf::Socket::Done && status != sf::Socket::Disconnected)
                {

                    cerr << "Error receiving reply from Server." << endl;
                    break;
                }
                else if (status == sf::Socket::Disconnected)
                {
                    prompt = "Disconnected";
                    cerr << "Unexpectedly disconnected from the server." << endl;
                    server_sock.disconnect();
                    connected = false;
                    continue;
                }

                std::string reply_str;
                reply >> reply_str;
                if(reply_str != "ack")
                {
                    cerr << "Operation unsuccessful. Server's reply: \n";
                    cerr << reply_str << endl;
                }
                else
                {
                    if(reply >> reply_str)
                        cout << reply_str << endl;
                }
                if(command == "shutdown")
                {
                    server_sock.disconnect();
                    connected = false;
                    prompt = "Disconnected";
                }
            }
            else
            {
                cerr << "Unrecognized local command and disconnected. Try \"help\"" << endl;
            }
        }
    }
    cout << endl;
    return 0;
}
