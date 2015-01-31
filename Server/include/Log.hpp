/**

    Logging for the server.

*/

#ifndef LOG_HPP
#define LOG_HPP


#include <iostream>
#include <fstream>
#include <string>

namespace prattle
{
    class Log
    {
        public:
            Log();
            ~Log();

            void addNewLogOutput(const std::string& log_message);
                                        // Adds a new line to server_log.txt about any important
                                        // events that occurs, e.g. modification of an object etc.

        private:
            std::ofstream m_logFile;
    };
}

#endif // LOG_HPP
