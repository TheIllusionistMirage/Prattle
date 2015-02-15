/**

    Prattle/Server/System.cpp
    =========================

    Contains implementations of functions declared in Prattle/server/System.hpp

*/

#include "System.hpp"

//#define LOG(stream, string) prattle::ErrorLog::Logger()->log(stream, string, __FILE__, __LINE__)

namespace prattle
{
    std::string getCurrentTimeAndDate()
    {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%r, %a %d %b %Y", &tstruct);
        return buf;
    }

    std::string generate_salt(int length)
    {
        std::random_device rand_dev;
        static const std::string allowed_chars{"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
        std::string salt;
        salt.resize(length);
        for(--length;length >= 0;--length)
        {
            salt[length] = allowed_chars[rand_dev()%allowed_chars.size()];
        }
        return salt;
    }

    std::string pwd_hash(const std::string& str,const std::string& salt)
    {
        return sha256(str+salt);
    }
}
