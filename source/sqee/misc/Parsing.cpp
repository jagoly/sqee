#include <sqee/misc/Parsing.hpp>

using namespace sq;

//============================================================================//

std::vector<StringView> sq::tokenise_string(StringView sv, char dlm)
{
    std::vector<StringView> result;

    while (true)
    {
        const auto begin = sv.find_first_not_of(dlm);
        if (begin == StringView::npos) { break; }
        sv.remove_prefix(begin);

        const auto end = sv.find_first_of(dlm);
        if (end == StringView::npos) { result.push_back(sv); break; }
        result.push_back({sv.data(), end});
        sv.remove_prefix(end + 1u);
    }

    return result;
}

//============================================================================//

std::vector<StringView> sq::tokenise_string_lines(StringView sv)
{
    // todo: maybe should handle other line endings, hasn't been a problem yet

    std::vector<StringView> result;

    while (sv.empty() == false)
    {
        const size_t lineEnd = sv.find('\n');
        result.push_back(sv.substr(0u, lineEnd));
        sv.remove_prefix(lineEnd + 1u);
    }

    return result;
}
