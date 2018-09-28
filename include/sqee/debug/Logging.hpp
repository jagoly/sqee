#pragma once

#include <sqee/misc/Builtins.hpp>
#include <sqee/misc/StringCast.hpp>

#include <sqee/redist/tinyformat.hpp>

namespace sq {

//============================================================================//

/// Get the system time in a tidy format.
SQEE_API String get_time_string();

//============================================================================//

/// Log an unformatted string without any prefix.
inline void log_only(const String& str)
{
    std::cout << str << std::endl;
}

/// Log a formatted string without any prefix.
template <class... Args>
inline void log_only(const char* fmt, const Args&... args)
{
    log_only(tfm::format(fmt, args...));
}

//============================================================================//

/// Log an unformatted INFO message.
inline void log_info(const String& str)
{
    std::cout << get_time_string() << " INFO: ";
    std::cout << str << std::endl;
}

/// Log a formatted INFO message.
template <class... Args>
inline void log_info(const char* fmt, const Args&... args)
{
    log_info(tfm::format(fmt, args...));
}

//============================================================================//

/// Log an unformatted WARNING message.
inline void log_warning(const String& str)
{
    std::cout << get_time_string() << " WARNING: ";
    std::cout << str << std::endl;
}

/// Log a formatted WARNING message.
template <class... Args>
inline void log_warning(const char* fmt, const Args&... args)
{
    log_warning(tfm::format(fmt, args...));
}

//============================================================================//

/// Log an unformatted ERROR message, then abort.
inline void log_error(const String& str)
{
    std::cout << get_time_string() << " ERROR: ";
    std::cout << str << std::endl; std::abort();
}

/// Log a formatted ERROR message, then abort.
template <class... Args>
inline void log_error(const char* fmt, const Args&... args)
{
    log_error(tfm::format(fmt, args...));
}

//============================================================================//

/// If not condition, log an unformatted ERROR message, then abort.
inline void log_assert(bool condition, const String& str)
{
    if (condition == true) return;

    std::cout << get_time_string() << " ERROR: ";
    std::cout << str << std::endl; std::abort();
}

/// If not condition, log a formatted ERROR message, then abort.
template <class... Args>
inline void log_assert(bool condition, const char* fmt, const Args&... args)
{
    log_assert(condition, tfm::format(fmt, args...));
}

//============================================================================//

/// Log a block of WARNING messages.
inline void log_warning_block(const String& heading, const Vector<String>& lines)
{
    std::cout << '\n' << get_time_string() << " WARNING: " << heading << '\n';
    for (const String& line : lines) std::cout << "  > " << line << '\n';
    std::cout << std::endl;
}

//============================================================================//

namespace literals {

struct FormatString final
{
    template <class... Args>
    String operator()(const Args&... args) const
    { return tfm::format(mFmtStr, args...); }

    const char* const mFmtStr;
};

inline FormatString operator ""_fmt_(const char* str, size_t)
{ return FormatString { str }; }

} // namespace literals

//============================================================================//

} // namespace sq
