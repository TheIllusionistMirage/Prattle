/**

    Prattle/Server/Logger.cpp
    ===========================

    Contains implementations of class Logger declared in Prattle/Server/Logger.hpp

*/

#include "../include/Logger.hpp"

namespace prattle
{
    Logger* Logger::instance = nullptr;

    Logger* Logger::getLogger()
    {
        if (instance == nullptr)
            instance = new Logger;
        return instance;
    }

    Logger::Logger() :
        m_logStream(&std::cout),
        m_level(Debug)
    {}
    Logger::~Logger()
    {
        m_logStream->flush();
    }
    void Logger::setStream(std::ostream* stream)
    {
        m_logStream = stream;
        (*m_logStream) << "------------ Prattle Server Logs --------------\n"
                       << "-------------- by team Prattle ----------------\n\n";
        m_logStream->flush();
    }

    void Logger::setLoggingLevel(LoggingLevel level)
    {
        m_level = level;
    }

    void Logger::error(const std::string& output,
             const std::string& file,
             unsigned int line)
    {
        (*m_logStream) << file << ":" << line << " :\n" << output << "\n\n";
        m_logStream->flush();
    }

    void Logger::debug(const std::string& output,
             const std::string& file,
             unsigned int line)
    {
        if (m_level <= Debug)
            (*m_logStream) << file << ":" << line << " :\n" << output << "\n\n";
        m_logStream->flush();
    }

    void Logger::warn(const std::string& output,
             const std::string& file,
             unsigned int line)
    {
        if (m_level <= Warning)
            (*m_logStream) << file << ":" << line << " :\n" << output << "\n\n";
        m_logStream->flush();
    }

}
