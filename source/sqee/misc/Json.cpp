#include <sqee/misc/Json.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

//============================================================================//

JsonValue sq::parse_json_from_file(const String& path)
{
    const auto text = read_text_from_file(path);

    return JsonValue::parse(text.data(), text.data() + text.length());
}

//============================================================================//

std::optional<JsonValue> sq::try_parse_json_from_file(const String& path)
{
    const auto text = try_read_text_from_file(path);

    // main use case for this function is files that might not exist
    if (text.has_value() == false)
        return std::nullopt;

    try { return JsonValue::parse(text->data(), text->data() + text->length()); }
    catch (const std::exception& e)
    {
        // we don't expect the json to fail to parse, so warn in this case
        log_warning_multiline("problem parsing json '{}'\nwhat(): {}", path, e.what());
        return std::nullopt;
    }
}

//============================================================================//

std::string fmt::formatter<sq::JsonValue>::impl_dump(const sq::JsonValue& json)
{
    std::string dumped = json.dump();

    if (json.is_structured() == false)
        return dumped;

    std::string result;

    char previous = '\0';
    bool quoted = false;

    for (const auto& character : dumped)
    {
        if (quoted == false)
        {
            if ((character == '}' && previous != '{') || (character == ']' && previous != '['))
                result.push_back(' ');

            else if ((previous == '{' && character != '}') || (previous == '[' && character != ']'))
                result.push_back(' ');

            else if (previous == ':' || previous == ',')
                result.push_back(' ');
        }

        result.push_back(character);

        if (character == '"' && previous != '\\')
            quoted = !quoted;

        previous = character;
    }

    return result;
}
