#include <sqee/misc/Parsing.hpp>

#include <sqee/debug/Assert.hpp>

using namespace sq;

//============================================================================//

std::vector<StringView> sq::tokenise_string(StringView sv, char dlm)
{
    std::vector<StringView> result;

    while (sv.empty() == false)
    {
        const size_t end = sv.find(dlm);

        if (end != 0u) result.emplace_back(sv.data(), end);
        if (end == StringView::npos) break;

        sv.remove_prefix(end + 1u);
    }

    return result;
}

//============================================================================//

void sq::tokenise_string(StringView sv, char dlm, std::vector<StringView>& out)
{
    out.clear(); // assume this overload is reusing the vector

    while (sv.empty() == false)
    {
        const size_t end = sv.find(dlm);

        if (end != 0u) out.push_back(sv.substr(0u, end));
        if (end == StringView::npos) break;

        sv.remove_prefix(end + 1u);
    }
}

//============================================================================//

std::vector<StringView> sq::tokenise_string_lines(StringView sv)
{
    // todo: maybe should handle other line endings, hasn't been a problem yet

    SQASSERT(sv.back() == '\n', "no newline at end of string");

    std::vector<StringView> result;

    while (sv.empty() == false)
    {
        const size_t end = sv.find('\n') + 1u;
        result.emplace_back(sv.data(), end);
        sv.remove_prefix(end);
    }

    return result;
}
