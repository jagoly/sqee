#pragma once

#ifdef SQEE_STATIC_LIB

    #define SQEE_API

#elif defined(SQEE_LINUX) && defined(SQEE_GNU)

    #define SQEE_API __attribute__ ((visibility ("default")))

#elif defined(SQEE_LINUX) && defined(SQEE_CLANG)

    #define SQEE_API __attribute__ ((visibility ("default")))

#elif defined(SQEE_WINDOWS) && defined(SQEE_GNU)

    #ifdef SQEE_EXPORT_LIB
        #define SQEE_API __attribute__ ((dllexport))
    #else
        #define SQEE_API __attribute__ ((dllimport))
    #endif

#elif defined(SQEE_WINDOWS) && defined(SQEE_MSVC)

    #ifdef SQEE_EXPORT_LIB
        #define SQEE_API __declspec(dllexport)
    #else
        #define SQEE_API __declspec(dllimport)
    #endif

#else

    #error "unsupported platform/compiler combo"

#endif
