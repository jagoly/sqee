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
        sv.remove_prefix(end);
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
