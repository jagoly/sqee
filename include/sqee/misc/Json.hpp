#include <sqee/builtins.hpp>

#include <sqee/redist/nl_json.hpp>

namespace sq {

using JsonValue = nlohmann::json;

JsonValue parse_json(const string& path);

} // namespace sq
