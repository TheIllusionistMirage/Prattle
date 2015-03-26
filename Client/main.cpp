#include <iostream>
#include <exception>
#include "include/ClientGUI.hpp"

using namespace std;

int main()
{
    try
    {
        cout << "=========================" << endl;
        cout << "   Prattle Client v0.1   " << endl;
        cout << "=========================" << endl;

        prattle::ClientGUI m_prattleClient;
        m_prattleClient.run(0.f);
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
