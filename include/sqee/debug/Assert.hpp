#pragma once

#ifndef NDEBUG

#include <iostream>

#define SQASSERT(condition, message)                            \
    do {                                                                    \
        if (!(condition))                                                           \
        {                                                                               \
            std::cerr << "assertion (" #condition ") failed, " << "in file '" << __FILE__ \
                      << "'\n    line " << __LINE__ << ": " << message << std::endl;      \
                                                                                        \
            std::abort();                                                           \
        }                                                                   \
    } while (false)                                             \

#else

#define SQASSERT(condition, message) do {} while (void(condition), false)

#endif
