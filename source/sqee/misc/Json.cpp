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
