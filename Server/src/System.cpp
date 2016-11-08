/*************************************************************
 *                                                           *
 *                Prattle/Server/System.cpp                  *
 *                                                           *
 *    ===================================================    *
 *                                                           *
 *    Contains implementations of helpers declared in        *
 *    Prattle/Server/System.hpp.                             *
 *                                                           *
 *    See https://github.com/TheIllusionistMirage/Prattle    *
 *    for more details.                                      *
 *                                                           *
 *************************************************************/


#if defined (_WIN32) && (defined (__MINGW32__) || defined (__MINGW64__))
    #include <sstream>

    // Declared in Prattle/Server/System.hpp

    int stoi(std::string str)
    {
        int i;
        std::stringstream ss(str);
        ss >> i;
        return i;
    }
#endif

#include "../include/System.hpp"
#include <ctime>    // for time
#include <cstdio>

namespace prattle
{
    // By following http://www.cplusplus.com/reference/ctime/strftime/
    std::string getCurrentTimeAndDate()
    {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%I:%m:%S %p, %a %d %b %Y", &tstruct);
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
