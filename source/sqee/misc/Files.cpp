// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/misc/Files.hpp>

#include <fstream>
#include <sstream>

#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>

using namespace sq;

//============================================================================//

bool sq::check_file_exists(const String& path)
{
    return std::ifstream(path).good();
}

//============================================================================//

String sq::get_string_from_file(const String& path)
{
    auto src = std::ifstream(path);

    if (src.good() == false)
    {
        log_warning("could not open file '%s'", path);
        return {};
    }

    std::stringstream stream;
    stream << src.rdbuf();
    return String(stream.str());
}

//============================================================================//

void sq::save_string_to_file(const String& path, const String& str)
{
    auto dest = std::ofstream(path);

    if (dest.good() == false)
    {
        log_warning("could not open file for writing '%s'", path);
        return;
    }

    dest << str;
}

//============================================================================//

Vector<std::byte> sq::get_bytes_from_file(const String& path)
{
    using unsigned_ifstream = std::basic_ifstream<std::byte>;
    unsigned_ifstream src(path, std::ios::binary | std::ios::ate);

    if (src.good() == false)
    {
        log_warning("Couldn't open file '%s'", path);
        return {};
    }

    const auto fileSize = src.tellg();

    Vector<std::byte> result;
    result.resize(uint(fileSize));

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

StringView sq::file_from_path(const String& path)
{
    const size_t splitPos = path.rfind('/');
    if (splitPos == path.size() - 1u) return StringView();
    return StringView(path.c_str() + splitPos + 1u);
}

StringView sq::directory_from_path(const String& path)
{
    const size_t splitPos = path.rfind('/');
    if (splitPos == String::npos) return StringView();
    return StringView(path.c_str(), splitPos);
}

StringView sq::extension_from_path(const String& path)
{
    const StringView fileName = file_from_path(path);
    const size_t dotPos = fileName.rfind('.', path.length());
    if (dotPos == String::npos) return StringView();
    if (dotPos == 0u) return StringView();
    return fileName.substr(dotPos);
}
