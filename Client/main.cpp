#include <iostream>
#include <exception>
#include <fstream>
#include "include/Client.hpp"
#include "include/Logger.hpp"
#include "include/TeeStream.hpp"


int main()
{
    try
    {
        std::ofstream logFile("prattle_client_log.txt");
        prattle::TeeStream tee(std::cerr, logFile);
        prattle::Logger::getLogger()->setStream(&tee);
        prattle::Logger::getLogger()->setLoggingLevel(prattle::Logger::Debug);

        prattle::Client m_prattleClient;
        m_prattleClient.run();
    }
    catch(std::exception& e)
    {
        ERR_LOG(std::string("Exception occurred.\nwhat(): ") + e.what());
    }

    return 0;
}
