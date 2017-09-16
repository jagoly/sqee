#include <fstream>
#include <sstream>

#include <sqee/assert.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

//============================================================================//

bool sq::check_file_exists(const string& path)
{
    return std::ifstream(path).good();
}

//============================================================================//

string sq::get_string_from_file(const string& path)
{
    if (std::ifstream src(path); src.is_open())
    {
        std::stringstream stream;
        stream << src.rdbuf();

        return stream.str();
    }

    log_warning("Couldn't open file %s", path);

    return {};
}

//============================================================================//

std::vector<uchar> sq::get_bytes_from_file(const string& path)
{
    using unsigned_ifstream = std::basic_ifstream<uchar>;
    unsigned_ifstream src(path, std::ios::binary | std::ios::ate);
    //std::ifstream src(path, std::ios::binary | std::ios::ate);

    if (src.is_open() == false)
    {
        log_warning("Couldn't open file '%s'", path);
        return {};
    }

    const auto fileSize = src.tellg();

    std::vector<uchar> result;
    result.resize(uint(fileSize));

    src.seekg(0, src.beg);
    src.read(result.data(), fileSize);

    return result;
}

//============================================================================//

std::vector<string> sq::tokenise_string(const string& str, char dlm)
{
    std::vector<string> result;

    std::stringstream stream ( str );
    string item;

    while (std::getline(stream, item, dlm))
        if (item.empty() == false)
            result.emplace_back(item);

    return result;
}

//============================================================================//

TokenisedFile sq::tokenise_file(const string& path)
{
    TokenisedFile result;

    if (std::ifstream src(path); src.is_open())
    {
        std::stringstream stream;
        stream << src.rdbuf();
        result.fullString = stream.str();
    }
    else { log_warning("Couldn't open file %s", path); return result; }

    const string& str = result.fullString;

    size_t tokenStart = 0u;
    size_t nextLine = str.find('\n');
    size_t nextSpace = str.find(' ');

    result.lines.push_back({ {}, 1u });

    while (nextLine != string::npos || nextSpace != string::npos)
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

string sq::file_name_from_path(const string& path)
{
    auto iter = std::find(path.rbegin(), path.rend(), '/');
    return string(iter.base(), path.end());
}

string sq::directory_from_path(const string& path)
{
    auto iter = std::find(path.rbegin(), path.rend(), '/');
    return string(path.begin(), iter.base());
}
