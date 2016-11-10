/*************************************************
 *                                               *
 *           Prattle/Client/Config.hpp           *
 *    =======================================    *
 *                                               *
 *    Holds OS specific code for implementing    *
 *    a minimum window size (800x600).           *
 *                                               *
 *************************************************/


#ifndef CONFIG_HPP
#define CONFIG_HPP

// *nix systems
#if defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD)
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #undef None
    #undef Status
#endif

// TODO
#if defined(_WIN32)
 // add WIN API code here
#endif

// If needed, any other OS's code will go here

#endif // CONFIG_HPP
