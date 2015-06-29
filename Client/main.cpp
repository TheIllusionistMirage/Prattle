#include <iostream>
#include <exception>
#include "include/Client.hpp"

using namespace std;

int main()
{
    try
    {
        cout << "=========================" << endl;
        cout << "   Prattle Client v0.1   " << endl;
        cout << "=========================" << endl;

        prattle::Client m_prattleClient;
        m_prattleClient.run(0.f);
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
