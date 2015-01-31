#include "Log.hpp"

namespace prattle
{
    Log::Log() :
        m_logFile("server_log.txt", std::ios::out | std::ios::trunc)
    {
        if (m_logFile.is_open())
        {
            m_logFile << "---  PRATTLE v 0.1  ---" << std::endl;
            m_logFile << "--- SERVER LOG FILE ---" << std::endl;
            m_logFile << std::endl;
        }
    }

    void Log::addNewLogOutput(const std::string& log_message)
    {
        if (m_logFile.is_open())
        {
            m_logFile << __FILE__ << " :: " << __LINE__ << log_message << std::endl;
        }
    }

    Log::~Log()
    {
        m_logFile << std::endl << "--- END OF LOG ---";
        m_logFile.close();
    }
}

