#include <sqee/misc/Files.hpp>

#include <sqee/core/Utilities.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>

#include <fstream>
#include <locale>
#include <sstream>

using namespace sq;

//============================================================================//

bool sq::check_file_exists(const String& path)
{
    return std::ifstream(path).good();
}

//============================================================================//

String sq::get_string_from_file(const String& path)
{
    // todo: profile and try to optimise file loading
    // STS is starting to take a while to load, especially on windows

    // todo: need to check or convert line endings
    // hasn't been a problem yet, but sqee can't deal with \r\n

    // todo: make this function abort or throw on failure
    // should use the try_ version most of the time

    auto src = std::ifstream(path, std::ios::in);

    if (src.good() == false)
    {
        log_warning("could not open file '{}'", path);
        return {};
    }

    std::stringstream stream;
    stream << src.rdbuf();
    return String(stream.str());
}

//============================================================================//

std::optional<String> sq::try_get_string_from_file(const String& path)
{
    if (auto src = std::ifstream(path, std::ios::in);
        src.good() == true)
    {
        std::stringstream stream;
        stream << src.rdbuf();
        return String(stream.str());
    }

    return std::nullopt;
}

//============================================================================//

void sq::save_string_to_file(const String& path, StringView str)
{
    auto dest = std::ofstream(path, std::ios::out);

    if (dest.good() == false)
    {
        log_warning("could not open file for writing '%s'", path);
        return;
    }

    dest << str;
}

//============================================================================//

std::vector<std::byte> sq::get_bytes_from_file(const String& path)
{
    auto src = std::basic_ifstream<std::byte>(path, std::ios::ate | std::ios::binary | std::ios::in);

    if (src.good() == false)
    {
        log_warning("could not open file '{}'", path);
        return {};
    }

    const auto fileSize = src.tellg();

    std::vector<std::byte> result;
    result.resize(size_t(fileSize));

    src.seekg(0, src.beg);
    src.read(result.data(), fileSize);

    return result;
}

//============================================================================//

TokenisedFile sq::tokenise_file(const String& path)
{
    TokenisedFile result;

    result.fullString = get_string_from_file(path);
    const String& str = result.fullString;

    if (str.empty()) return result;

    size_t tokenStart = 0u;
    size_t nextLine = str.find('\n');
    size_t nextSpace = str.find(' ');

    result.lines.push_back({ {}, 1u });

    while (nextLine != String::npos || nextSpace != String::npos)
    {
        if (nextSpace < nextLine)
        {
            if (size_t tokenLen = nextSpace - tokenStart; tokenLen != 0u)
            {
                const char* startPtr = str.data() + tokenStart;
                result.lines.back().tokens.emplace_back(startPtr, tokenLen);
            }

            tokenStart = ++nextSpace;
            nextSpace = str.find(' ', nextSpace);
        }
        else if (nextLine < nextSpace)
        {
            if (size_t tokenLen = nextLine - tokenStart; tokenLen != 0u)
            {
                const char* startPtr = str.data() + tokenStart;
                result.lines.back().tokens.emplace_back(startPtr, tokenLen);
            }

            tokenStart = ++nextLine;
            nextLine = str.find('\n', nextLine);

            if (result.lines.back().tokens.empty() == false)
            {
                size_t prevLineNum = result.lines.back().num;
                result.lines.emplace_back();
                result.lines.back().num = prevLineNum;
            }

            ++result.lines.back().num;
        }
        else SQASSERT(false, "");
    }

    if (size_t tokenLen = str.length() - tokenStart; tokenLen != 0u)
    {
        const char* startPtr = str.data() + tokenStart;
        result.lines.back().tokens.emplace_back(startPtr, tokenLen);
    }

    if (result.lines.back().tokens.empty())
        result.lines.pop_back();

    return result;
}

//============================================================================//

StringView sq::file_from_path(StringView path)
{
    const size_t splitPos = path.rfind('/');
    if (splitPos == path.size() - 1u) return StringView();
    return path.substr(splitPos + 1u);
}

StringView sq::directory_from_path(StringView path)
{
    const size_t splitPos = path.rfind('/');
    if (splitPos == StringView::npos) return StringView();
    return path.substr(0u, splitPos + 1u);
}

StringView sq::extension_from_path(StringView path)
{
    const StringView fileName = file_from_path(path);
    const size_t dotPos = fileName.rfind('.');
    if (dotPos == StringView::npos) return StringView();
    if (dotPos == 0u) return StringView();
    return fileName.substr(dotPos);
}

//============================================================================//

String sq::compute_resource_path(StringView key,
                                 std::initializer_list<StringView> prefixes,
                                 std::initializer_list<StringView> extensions)
{
    SQASSERT(key.front() != '/', "resource keys cannot be absolute");
    SQASSERT(extension_from_path(key).empty(), "resource keys cannot have extensions");
    SQASSERT(prefixes.size() > 0u, "no prefixes given");
    SQASSERT(extensions.size() > 0u, "no extensions given");

    for (const StringView& prefix : prefixes)
    {
        SQASSERT(!prefix.empty() && prefix.back() == '/', "invalid prefix");

        for (const StringView& extension : extensions)
        {
            SQASSERT(!extension.empty() && extension.front() == '.', "invalid extension");

            String result = build_string(prefix, key, extension);
            if (check_file_exists(result)) return result;
        }
    }

    // we return the original string so that the resources themselves
    // can use it to print a useful error message
    return String(key);
}
