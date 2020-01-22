// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/misc/Parsing.hpp>

using namespace sq;

//============================================================================//

Vector<StringView> sq::tokenise_string_view(StringView sv, char dlm)
{
    Vector<StringView> result;

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

Vector<Pair<uint, StringView>> sq::tokenise_string_view_lines(StringView sv)
{
    Vector<Pair<uint, StringView>> result;

    // line counting usually starts from one
    for (uint line = 1u;; ++line)
    {
        const auto begin = sv.find_first_not_of('\n');
        if (begin == StringView::npos) { break; }
        line += uint(begin);
        sv.remove_prefix(begin);

        const auto end = sv.find_first_of('\n');
        if (end == StringView::npos) { result.emplace_back(line, sv); break; }
        result.emplace_back(line, StringView{sv.data(), end});
        sv.remove_prefix(end + 1u);
    }

    return result;
}

//============================================================================//

TokenisedString sq::tokenise_string(String str, char dlm)
{
    TokenisedString result;

    result.fullString = std::move(str);
    result.tokens = tokenise_string_view(result.fullString, dlm);

    return result;
}
