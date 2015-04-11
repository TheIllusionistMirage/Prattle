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
    std::vector<std::string> words;
    std::stringstream ss(str);
    std::string word;
    while(ss >> word)
    {
        words.push_back(word);
    }
    return words;
}

int main()
{
    sf::TcpSocket server_sock;
    std::string input, prompt = "Disconnected";
    const std::string prompt_suffix = " > ";
    bool connected = false;

    cout << prompt+prompt_suffix;
    while(std::getline(std::cin, input))
    {
        if(input.empty())
        {
            continue;
        }

        auto words = split(input);
        std::string command = words[0];
        if(command == "exit")
        {
            server_sock.disconnect();
            break;
        }
        else if(command == "help")
        {
            cout << "Server-Controller, by team Prattle." << endl
                  << "Use this to control a Prattle Server." << endl
                  << "Available commands : " << endl;
            //Raw string, because I'm too lazy
            cout << R"(Local commands for Server-Controller

Command              Description
-----------------------------------------------------------------
help                 Print help
connect <ip>:<port>  Connect to Server at IP <ip> and port <port>
disconnect           Disconnect from Server
exit                 Disconnect (if connected) and exit


Commands sent to the server.

Command            Description
-------------------------------------------------------------------------
shutdown           Shutdown the server
show_logged_users  Prints the usernames of all logged users
remove_user <name> Removes user <name> from the DB
print_stats        Prints stats about the server (uptime, number of users etc.))";

        }
        else if(command == "connect")
        {
            if(connected)
            {
                cerr << "Invalid Command.\nAlready connected to server at " << server_sock.getRemoteAddress() << endl;
                continue;
            }
            if(words.size() < 3)
                cerr << "Invalid command." << endl;
            std::string ip = words[1];
            unsigned short port = std::stoi(words[2]);
            auto status = server_sock.connect(ip, port, sf::seconds(60));
            if(status != sf::Socket::Done)
            {
                cerr << "ERROR connecting to server." << endl;
            }
            else
            {
                sf::Packet connect_packet;
                std::string passphrase;
                cout << "Enter the passphrase for this server : ";
                std::cin >> passphrase;
                connect_packet << "controller_attach" << passphrase;
                status = server_sock.send(connect_packet);
                if(status != sf::Socket::Done)
                {
                    cerr << "ERROR while sending packet to server." << endl;
                }
                else
                {
                    sf::Packet connect_reply;
                    status = server_sock.receive(connect_reply);
                    if(status != sf::Socket::Done)
                    {
                        cerr << "ERROR while receiving to server. Status code : " << status << endl;
                    }
                    else
                    {
                        prompt = ip;
                        connected = true;
                        cout << "Connected.\n";
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
                for(auto& s : words)
                {
                    packet << s;
                }
                auto status = server_sock.send(packet);
                if(status != sf::Socket::Done)
                {
                    cerr << "Error sending packet to server." << endl;
                    continue;
                }
                sf::Packet reply;
                status = server_sock.receive(reply);
                if(status != sf::Socket::Done)
                {
                    cerr << "Error receiving reply from Server." << endl;
                    break;
                }
                std::string reply_str;
                reply >> reply_str;
                if(reply_str != "ack")
                {
                    cerr << "Operation unsuccessful. Server's reply : \n";
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

//        std::cin.ignore(std::numeric_limits<std::streamsize>::max()); //Ignore anything typed between last prompt and the next
        cout << prompt+prompt_suffix;
    }
    cout << endl;
    return 0;
}
