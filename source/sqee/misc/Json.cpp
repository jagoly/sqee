#include <sqee/misc/Json.hpp>

#include <sqee/debug/Logging.hpp>

#include <fstream>

using namespace sq;

//============================================================================//

JsonValue sq::parse_json_from_file(const String& path)
{
    auto src = std::ifstream(path);

    if (src.good() == false)
    {
        log_warning("could not open json file '%s'", path);
        return {};
    }

    try { return JsonValue::parse(src); }
    catch (const std::exception& e)
    {
        log_warning("problem parsing json '%s'\n  what():  %s", path, e.what());
        return {};
    }
}

//============================================================================//

void sq::from_json(const JsonValue& j, TinyString& str)
{
    if (j.is_string() == false || j.size() > TinyString::capacity())
        std::invalid_argument(sq::build_string("from_json: ", j.dump(), " -> TinyString"));

    str = j.get_ref<const String&>();
}

void sq::to_json(JsonValue& j, const TinyString& str)
{
    j = String(str);
}
