#pragma once

#include <sqee/assert.hpp>
#include <sqee/misc/Parsing.hpp>

#include <sqee/redist/nl_json.hpp>

namespace sq {

//============================================================================//

using JsonValue = nlohmann::json;

JsonValue parse_json_from_file(const string& path);

//============================================================================//

template <int S, class T>
inline void from_json(const JsonValue& json, Vector<S, T>& out)
{
    // fix this when json lib has proper support for string view
    const auto tokens = tokenise_string_view(json.get_ref<const string&>(), ' ');
    SQASSERT(tokens.size() == S, "invalid conversion");

    if constexpr (S == 2) parse_tokens(out, tokens[0], tokens[1]);
    if constexpr (S == 3) parse_tokens(out, tokens[0], tokens[1], tokens[2]);
    if constexpr (S == 4) parse_tokens(out, tokens[0], tokens[1], tokens[2], tokens[3]);
}

//============================================================================//

} // namespace sq
