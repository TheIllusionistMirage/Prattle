#include <iostream>
#include <exception>
#include "include/Client.hpp"
#include "include/Logger.hpp"
#include "include/TeeStream.hpp"


int main()
{
    try
    {
        std::ofstream logFile("prattle_log.txt");
        prattle::TeeStream tee(std::cerr, logFile);
        prattle::Logger::getLogger()->setStream(&tee);
        prattle::Logger::getLogger()->setLoggingLevel(prattle::Logger::Debug);

        prattle::Client m_prattleClient;
        m_prattleClient.run();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
