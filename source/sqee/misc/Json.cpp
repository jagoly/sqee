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
        log_warning("could not open json file '{}'", path);
        return JsonValue();
    }

    try { return JsonValue::parse(src); }
    catch (const std::exception& e)
    {
        log_warning_multiline("problem parsing json '{}'\nwhat(): {}", path, e.what());
        return JsonValue();
    }
}
