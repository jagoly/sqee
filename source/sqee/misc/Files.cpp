#include <sqee/misc/Files.hpp>

#include <sqee/debug/Logging.hpp>

using namespace sq;

//============================================================================//

String sq::read_text_from_file(const String& path)
{
    FILE* file = std::fopen(path.c_str(), "r");

    if (file == NULL)
        SQEE_THROW("could not read file '{}'", path);

    std::fseek(file, 0, SEEK_END);
    const size_t fileSize = std::ftell(file);

    auto buffer = std::vector<char>(fileSize);
    std::fseek(file, 0, SEEK_SET);
    const size_t stringSize = std::fread(buffer.data(), 1u, buffer.size(), file);
    std::fclose(file);

    return String(buffer.data(), stringSize);
}

//============================================================================//

std::vector<std::byte> sq::read_bytes_from_file(const String& path)
{
    FILE* file = std::fopen(path.c_str(), "rb");

    if (file == NULL)
        SQEE_THROW("could not read file '{}'", path);

    std::fseek(file, 0, SEEK_END);
    const size_t fileSize = std::ftell(file);

    auto buffer = std::vector<std::byte>(fileSize);
    std::fseek(file, 0, SEEK_SET);
    std::fread(buffer.data(), 1u, buffer.size(), file);
    std::fclose(file);

    return buffer;
}

//============================================================================//

std::optional<String> sq::try_read_text_from_file(const String& path)
{
    FILE* file = std::fopen(path.c_str(), "r");

    if (file == NULL)
        return std::nullopt;

    std::fseek(file, 0, SEEK_END);
    const size_t fileSize = std::ftell(file);

    auto buffer = std::vector<char>(fileSize);
    std::fseek(file, 0, SEEK_SET);
    const size_t stringSize = std::fread(buffer.data(), 1u, buffer.size(), file);
    std::fclose(file);

    return String(buffer.data(), stringSize);
}

//============================================================================//

std::optional<std::vector<std::byte>> sq::try_read_bytes_from_file(const String& path)
{
    FILE* file = std::fopen(path.c_str(), "rb");

    if (file == NULL)
        return std::nullopt;

    std::fseek(file, 0, SEEK_END);
    const size_t fileSize = std::ftell(file);

    auto buffer = std::vector<std::byte>(fileSize);
    std::fseek(file, 0, SEEK_SET);
    std::fread(buffer.data(), 1u, buffer.size(), file);
    std::fclose(file);

    return buffer;
}

//============================================================================//

void sq::write_text_to_file(const String& path, StringView text, bool endNewLine)
{
    FILE* file = std::fopen(path.c_str(), "w");

    if (file == NULL)
        SQEE_THROW("could not write file '{}'", path);

    std::fwrite(text.data(), 1u, text.size(), file);

    if (endNewLine == true && text.back() != '\n')
        std::fputc('\n', file);

    std::fclose(file);
}

//============================================================================//

void sq::write_bytes_to_file(const String& path, const void* bytes, size_t size)
{
    FILE* file = std::fopen(path.c_str(), "wb");

    if (file == NULL)
        SQEE_THROW("could not write file '{}'", path);

    std::fwrite(bytes, 1u, size, file);
    std::fclose(file);
}

//============================================================================//

TokenisedFile TokenisedFile::from_file(const String& path)
{
    return from_string(read_text_from_file(path));
}

//============================================================================//

TokenisedFile TokenisedFile::from_string(String&& text)
{
    TokenisedFile result;

    result.source = std::move(text);
    if (result.source.empty()) return result;

    size_t tokenStart = 0u;
    size_t nextLine = result.source.find('\n');
    size_t nextSpace = result.source.find(' ');

    result.lines.push_back({ {}, 1u });

    while (nextLine != String::npos || nextSpace != String::npos)
    {
        if (nextSpace < nextLine)
        {
            if (size_t tokenLen = nextSpace - tokenStart; tokenLen != 0u)
            {
                const char* startPtr = result.source.data() + tokenStart;
                result.lines.back().tokens.emplace_back(startPtr, tokenLen);
            }

            tokenStart = ++nextSpace;
            nextSpace = result.source.find(' ', nextSpace);
        }
        else
        {
            if (size_t tokenLen = nextLine - tokenStart; tokenLen != 0u)
            {
                const char* startPtr = result.source.data() + tokenStart;
                result.lines.back().tokens.emplace_back(startPtr, tokenLen);
            }

            tokenStart = ++nextLine;
            nextLine = result.source.find('\n', nextLine);

            if (result.lines.back().tokens.empty() == false)
            {
                size_t prevLineNum = result.lines.back().lineNum;
                result.lines.emplace_back();
                result.lines.back().lineNum = prevLineNum;
            }

            ++result.lines.back().lineNum;
        }
    }

    if (size_t tokenLen = result.source.length() - tokenStart; tokenLen != 0u)
    {
        const char* startPtr = result.source.data() + tokenStart;
        result.lines.back().tokens.emplace_back(startPtr, tokenLen);
    }

    if (result.lines.back().tokens.empty())
        result.lines.pop_back();

    return result;
}
