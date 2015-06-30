#include <fstream>
#include "include/Server.hpp"
#include "include/Logger.hpp"
#include "include/TeeStream.hpp"

int main()
{
    try
    {
        std::ofstream logFile("server_log.txt");
        prattle::TeeStream tee(std::cerr, logFile);
        prattle::Logger::getLogger()->setStream(&tee);
        prattle::Logger::getLogger()->setLoggingLevel(prattle::Logger::Debug);
        prattle::Server server;
        server.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
