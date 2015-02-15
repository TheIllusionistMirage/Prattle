/**

    Prattle/Server/ErrorLog
    =======================

    Provides error logging with various layers of error:
     - WARNINGs
     - SERVER_ERRORs
     - CLIENT_ERRORs
     - FATAL_ERRORs

    The results of the log file can be found in 'logFile.txt'.

*/

#ifndef ERRORLOG_HPP
#define ERRORLOG_HPP

#include <iostream>
#include <fstream>
#include <sstream>

namespace prattle
{
    #define LOG(output) ErrorLog::Logger()->log(output, __FILE__, __LINE__)
    #define STREAM "file"   // <==== Change this as and when required to change the output streams
                            // - STDOUT- for std output and FILE for file

    const std::string LOG_FILE = "server_log.txt";    // Error log file

    class ErrorLog
    {
        public:
            static ErrorLog* Logger();

            void log(const std::string& output,
                     const std::string& file,
                     unsigned int line,
                     const std::string& strm = STREAM);

        protected:
        private:
            ErrorLog();
            ~ErrorLog();
            std::ofstream logFile;
            std::string buffer;
            unsigned short lineNo;
            static ErrorLog* instance;
    };
}

#endif // ERRORLOG_HPP
