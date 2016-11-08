/******************************************************************
 *                                                                *
 *                    Prattle/Server/Logger.cpp                   *
 *    ========================================================    *
 *                                                                *
 *    Contains implementation of class Logger declared in         *
 *    Prattle/Server/Logger.hpp.                                  *
 *                                                                *
 *    See https://github.com/TheIllusionistMirage/Prattle for     *
 *    more details.                                               *
 *                                                                *
 ******************************************************************/


#include "../include/Logger.hpp"
#include "../include/System.hpp"

namespace prattle
{
    Logger* Logger::m_instance = nullptr;

    Logger* Logger::getLogger()
    {
        if (m_instance == nullptr)
            m_instance = new Logger;

        return m_instance;
    }

    void Logger::setStream(std::ostream* stream)
    {
        m_logStream = stream;

        (*m_logStream) << prattle_logo
          << "\n\n\n"
          << "------------------------Server Logs----------------------\n"
          << "----------------------by team Prattle -------------------\n\n\n";

        m_logStream->flush();
    }

    void Logger::setLoggingLevel(LoggingLevel level)
    {
        m_level = level;
    }

    void Logger::debug(const std::string& output ,
                       const std::string& file   ,
                       unsigned int line)
    {
        if (m_level <= Debug)
        {
            (*m_logStream) << file
                           << ":"
                           << line
                           << " :\n"
                           << output
                           << "\n\n";
        }

        m_logStream->flush();
    }

    void Logger::warn(const std::string& output ,
                      const std::string& file   ,
                      unsigned int line)
    {
        if (m_level <= Warning)
            (*m_logStream) << file
                           << ":"
                           << line
                           << " :\n"
                           << output
                           << "\n\n";

        m_logStream->flush();
    }

    void Logger::error(const std::string& output ,
                       const std::string& file   ,
                       unsigned int line)
    {
        (*m_logStream) << file
                       << ":"
                       << line
                       << " :\n"
                       << output
                       << "\n\n";

        m_logStream->flush();
    }

    void Logger::release()
    {
        if (m_instance != nullptr)
            delete m_instance;
    }

    Logger::Logger() : m_logStream(&std::cout) ,
                       m_level(Debug)
    {}

    Logger::~Logger()
    {
        m_logStream->flush();
    }
}
