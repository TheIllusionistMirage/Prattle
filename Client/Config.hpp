/**

    Prattle/Client/Config.hpp

    Holds OS specific includes/codes for implementing a minimum window size.

*/

#ifndef CONFIG_HPP
#define CONFIG_HPP

#if defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD)
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #undef None
    #undef Status
#endif

/** TO DO */
#if defined(_WIN32)
 // add WIN API code here
#endif

// If needed, any other OS's code will go here

#endif // CONFIG_HPP_INCLUDED
