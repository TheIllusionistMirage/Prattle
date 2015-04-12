/**

    Prattle/Server/ErrorLog.hpp
    ===========================

    Provides a very basic error logging system.The results of the log file can
    be found in 'server_log.txt' in the directory containing the Server binary.

*/

#ifndef ERRORLOG_HPP
#define ERRORLOG_HPP

#include <iostream>
#include <fstream>
#include <sstream>

namespace prattle
{
    #define LOG(output) prattle::ErrorLog::Logger()->log(output, __FILE__, __LINE__)
    #define STREAM "stdout" // <== Change this as and when required to change the output streams
                            // - "stdout" - for std output AND file
                            // - "file"   - for file

    const std::string LOG_FILE = "server_log.txt";    // Error log file

    // Class ErrorLog which provides methods to log errors,
    // warnings or any test messages to the log file.
    //
    // The ErrorLog class is a singleton class which ensures
    // only one instance of the logger is created.

    class ErrorLog
    {
        public:
            static ErrorLog* Logger();

            void log(const std::string& output,
                     const std::string& file,
                     unsigned int line,
                     const std::string& stream = STREAM);   // Log a line of information to STREAM

        protected:
        private:
            ErrorLog();
            ~ErrorLog();
            std::ofstream logFile;
            std::string buffer;
            static ErrorLog* instance;
    };
}

#endif // ERRORLOG_HPP
