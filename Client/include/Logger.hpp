#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <memory>

// Macros for logging, they also add the filename and line
// For printing debugging/diagnostic information like system state, sockets etc
// and checking whether things are working as expected i.e. monitoring control flow etc.
#define DBG_LOG(output) prattle::Logger::getLogger()->debug(output, __FILE__, __LINE__)
// An error has occurred
#define ERR_LOG(output) prattle::Logger::getLogger()->error(output, __FILE__, __LINE__)
// Warning
#define WRN_LOG(output) prattle::Logger::getLogger()->warn(output, __FILE__, __LINE__)

// Macro which makes the identifier/symbol x as a string
#define SYM_STR(X) (#X)

namespace prattle
{
    // Class Logger which provides methods to log errors,
    // warnings or any test messages to the log file.
    //
    // The Logger class is a singleton class which ensures
    // only one instance of the logger is created.
    //
    //  If the logging level is set to Error, only errors are printed/output
    //  If the logging level is Debug, only Debug and error logs are printed.

    class Logger
    {
        public:
            enum LoggingLevel
            {
                Debug,
                Warning,
                Error,
            };
            static Logger* getLogger();
            void setStream(std::ostream* stream);
            void setLoggingLevel(LoggingLevel level);
            //An error has occurred
            void error(const std::string& output,
                     const std::string& file,
                     unsigned int line);
            //For printing debugging/diagnostic information like system state, sockets etc
            void debug(const std::string& output,
                     const std::string& file,
                     unsigned int line);
            //Warnings
            void warn(const std::string& output,
                     const std::string& file,
                     unsigned int line);

        protected:
        private:
            Logger();
            ~Logger();

            std::ostream* m_logStream;
            LoggingLevel m_level;

            static Logger* instance;
    };
}

#endif // LOGGER_HPP
