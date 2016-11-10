/******************************************************************
 *                                                                *
 *                    Prattle/Client/Logger.hpp                   *
 *    ========================================================    *
 *                                                                *
 *    Provides a very basic error logging system. The results     *
 *    of the log file can be found in 'prattle_server_log.txt'    *
 *    in the directory containing the prattle-server binary.      *
 *                                                                *
 *    See https://github.com/TheIllusionistMirage/Prattle for     *
 *    more details.                                               *
 *                                                                *
 ******************************************************************/


#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <memory>

// Macros for logging, they also add the filename, line and function
// For printing debugging/diagnostic information like system state,
// sockets etc. and checking whether things are working as expected,
// i.e., monitoring control flow etc.

// Log a general debug info
#define DBG_LOG(output) prattle::Logger::getLogger()->debug(output, __FILE__, __LINE__)

// Log a warning
#define WRN_LOG(output) prattle::Logger::getLogger()->warn(output, __FILE__, __LINE__)

// Log an error
#define ERR_LOG(output) prattle::Logger::getLogger()->error(output, __FILE__, __LINE__)

// Macro which makes the identifier/symbol x as a string
#define SYM_STR(X) (#X)

namespace prattle
{
    ///////////////////////////////////////////////////////////
    //                                                       //
    //  Class Logger which provides methods to log errors,   //
    //  warnings or any test messages to the log file.       //
    //                                                       //
    //  The Logger class is a singleton class which ensures  //
    //  only one instance of the logger is created.          //
    //                                                       //
    //  If the logging level is set to Error, only errors    //
    //  are printed/output to cerr. If the logging level is  //
    //  Debug, only Debug and error logs are printed.        //
    //                                                       //
    ///////////////////////////////////////////////////////////

    class Logger
    {
        public:

            enum LoggingLevel
            {
                Debug,
                Warning,
                Error,
            };

            // Creates a new instance of Logger if one doesn't already
            // exist. Else returns a pointer to the existing instance
            static Logger* getLogger();

            // Set the logging stream
            void setStream(std::ostream* stream);

            // Set the logging level
            // - Level Debug prints both debug and warnings/errors
            // - Level Error prints only warnings/errors
            void setLoggingLevel(LoggingLevel level);

            // Log a debug info
            void debug(const std::string& output ,
                       const std::string& file   ,
                       unsigned int line);

            // Log a warning
            void warn(const std::string& output ,
                      const std::string& file   ,
                      unsigned int line);

            // Log an error
            void error(const std::string& output ,
                       const std::string& file   ,
                       unsigned int line);

            // Destory the logger instance
            static void release();

        private:

                           Logger();
                          ~Logger();

            // The log output stream
            std::ostream*  m_logStream;

            // The log level (Debug or Error)
            LoggingLevel   m_level;

            static Logger* m_instance;
    };
}

#endif // LOGGER_HPP
