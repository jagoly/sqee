#include <sqee/debug/Logging.hpp>

#include <fmt/chrono.h>

using namespace sq;

//============================================================================//

void sq::detail::log_time_info()
{
    std::time_t now = std::time(nullptr);
    fmt::print("{:%H:%M:%S} INFO: ", fmt::localtime(now));
}

void sq::detail::log_time_warning()
{
    std::time_t now = std::time(nullptr);
    fmt::print("{:%H:%M:%S} WARNING: ", fmt::localtime(now));
}

void sq::detail::log_time_error()
{
    std::time_t now = std::time(nullptr);
    fmt::print("{:%H:%M:%S} ERROR: ", fmt::localtime(now));
}

void sq::detail::log_time_debug()
{
    std::time_t now = std::time(nullptr);
    fmt::print("{:%H:%M:%S} DEBUG: ", fmt::localtime(now));
}

//============================================================================//

void sq::detail::log_multiline(StringView str)
{
    size_t lineEnd = str.find('\n', 0u);

    #ifdef SQEE_DEBUG
    if (lineEnd == 0u || lineEnd == StringView::npos)
        log_error("invalid multiline string");
    #endif

    std::fwrite(str.data(), 1u, lineEnd, stdout);
    size_t lineStart = lineEnd + 1u;

    while (str[lineStart] != '\0')
    {
        std::fputs("\n  > ", stdout);

        lineEnd = str.find('\n', lineStart);
        auto line = str.substr(lineStart, lineEnd - lineStart);
        std::fwrite(line.data(), 1u, line.size(), stdout);

        if (lineEnd == StringView::npos) break;
        lineStart = lineEnd + 1u;
    }

    std::fputc('\n', stdout);
    std::fflush(stdout);
}
