/**

    Prattle/Client/ErrorLog.cpp
    ===========================

    Contains implementations of class ErrorLog declared in Prattle/Client/ErrorLog.hpp

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
        logFile(LOG_FILE, std::ios::out | std::ios::trunc),
        lineNo(1)
    {
        if (logFile.is_open())
        {
            logFile << "---  PRATTLE v 0.1  ---" << std::endl;
            logFile << "--- SERVER LOG FILE ---\n" << std::endl;
            log("Logger started", __FILE__, __LINE__);
        }
    }

    void ErrorLog::log(const std::string& output, const std::string& file, unsigned int line, const std::string& stream)
    {
        if (stream == "file")
        {
            if (logFile.is_open())
                logFile << lineNo++ << ".)\n File :" << file << ":\n Line :" << line << "\n --- " << output << "\n" << std::endl;
        }

        else if (stream == "stdout")
        {

            logFile << lineNo << ".)\n File :" << file << ":\n Line : " << line << "\n --- " << output << " [Also logged to STDOUT]\n" << std::endl;
            std::cout << lineNo++ << ".)\n File :" << file << ":\n Line :" << line << "\n --- " << output << "\n" << std::endl;
        }
    }

    ErrorLog::~ErrorLog()
    {
        logFile.close();
    }
}
