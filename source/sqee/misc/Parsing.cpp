#include <sqee/misc/Parsing.hpp>

using namespace sq;

//============================================================================//

std::vector<string_view> sq::tokenise_string_view(string_view sv, char dlm)
{
    std::vector<string_view> result;

    while (true)
    {
        const auto begin = sv.find_first_not_of(dlm);
        if (begin == string_view::npos) { break; }
        sv.remove_prefix(begin);

        const auto end = sv.find_first_of(dlm);
        if (end == string_view::npos) { result.push_back(sv); break; }
        result.push_back({sv.data(), end});
        sv.remove_prefix(end);
    }

    return result;
}

//============================================================================//

TokenisedString sq::tokenise_string(string str, char dlm)
{
    TokenisedString result;

    result.fullString = std::move(str);
    result.tokens = tokenise_string_view(result.fullString, dlm);

    return result;
}
