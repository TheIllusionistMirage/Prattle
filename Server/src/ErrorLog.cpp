/**

    Prattle/Server/ErrorLog.cpp
    ===========================

    Contains implementations of class ErrorLog declared in Prattle/Server/ErrorLog.hpp

*/

#include "../include/ErrorLog.hpp"

namespace prattle
{
    ErrorLog* ErrorLog::instance = nullptr;

    ErrorLog* ErrorLog::Logger()
    {
        if (instance == nullptr)
            instance = new ErrorLog;
        return instance;
    }

    ErrorLog::ErrorLog() :
        logFile(LOG_FILE, std::ios_base::out | std::ios_base::trunc)
    {
        if (logFile.is_open())
        {
            std::cout << "Prattle v0.1" << std::endl;
            std::cout << "Server" << std::endl;
            std::cout << "Logs : " << std::endl;
            logFile << "Prattle v0.1" << std::endl;
            logFile << "Server" << std::endl;
            logFile << "Logs : " << std::endl;
        }
    }

    void ErrorLog::log(const std::string& output, const std::string& file, unsigned int line, const std::string& stream)
    {
        if (stream == "file")
        {
            if (logFile.is_open())
                logFile << file << ":" << line << " :\n\t" << output << "\n";
        }

        else if (stream == "stdout")
        {
            logFile << file << ":" << line << " :\n\t" << output << "\n";
            std::cout <<file << ":" << line << " :\n\t" << output << "\n";
        }
    }

    ErrorLog::~ErrorLog()
    {
        logFile.close();
    }
}
