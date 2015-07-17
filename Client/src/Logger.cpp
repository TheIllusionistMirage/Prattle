#include "../include/Logger.hpp"
#include "../include/System.hpp"

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
        (*m_logStream) << prattle_logo << "\n\n\n"
                       << "------------------------Client Logs----------------------\n"
                       << "----------------------by team Prattle -------------------\n\n\n";
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
