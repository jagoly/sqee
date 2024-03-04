#include <sqee/debug/Logging.hpp>

#include <fmt/chrono.h>

#include <cstdio>
#include <ctime>
#include <string>

using namespace sq;

//============================================================================//

inline void impl_append_time_and_category(std::string_view category, fmt::memory_buffer& buffer)
{
    if (category.empty()) return;

    const std::time_t now = std::time(nullptr);
    fmt::format_to(fmt::appender(buffer), "{:%H:%M:%S} {}: ", fmt::localtime(now), category);
}

inline void impl_write_multiline(const fmt::memory_buffer& buffer, FILE* file)
{
    using Traits = std::char_traits<char>;

    const char* const bufferEnd = buffer.end();

    const char* lineStart = buffer.begin();
    const char* lineEnd = Traits::find(lineStart, size_t(bufferEnd - lineStart), '\n') + 1;

    std::fwrite(lineStart, 1u, size_t(lineEnd - lineStart), file);

    while (lineEnd != bufferEnd)
    {
        lineStart = lineEnd;
        lineEnd = Traits::find(lineStart, size_t(bufferEnd - lineStart), '\n') + 1;

        std::fwrite("  > ", 1u, 4u, file);
        std::fwrite(lineStart, 1u, size_t(lineEnd - lineStart), file);
    }
}

//============================================================================//

void sq::detail::log(std::string_view category, std::string_view str)
{
    fmt::memory_buffer buffer;

    impl_append_time_and_category(category, buffer);

    buffer.append(str);
    buffer.push_back('\n');

    std::fwrite(buffer.data(), 1u, buffer.size(), stdout);
    std::fflush(stdout);
}

void sq::detail::log_multiline(std::string_view category, std::string_view str)
{
    fmt::memory_buffer buffer;

    impl_append_time_and_category(category, buffer);

    buffer.append(str);
    buffer.push_back('\n');

    impl_write_multiline(buffer, stdout);
    std::fflush(stdout);
}

//============================================================================//

void sq::detail::log(std::string_view category, fmt::string_view fstr, fmt::format_args args)
{
    fmt::memory_buffer buffer;

    impl_append_time_and_category(category, buffer);

    fmt::vformat_to(fmt::appender(buffer), fstr, args);
    buffer.push_back('\n');

    std::fwrite(buffer.data(), 1u, buffer.size(), stdout);
    std::fflush(stdout);
}

void sq::detail::log_multiline(std::string_view category, fmt::string_view fstr, fmt::format_args args)
{
    fmt::memory_buffer buffer;

    impl_append_time_and_category(category, buffer);

    fmt::vformat_to(fmt::appender(buffer), fstr, args);
    buffer.push_back('\n');

    impl_write_multiline(buffer, stdout);
    std::fflush(stdout);
}
