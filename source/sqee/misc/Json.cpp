#include <fstream>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Json.hpp>

using namespace sq;

//============================================================================//

JsonValue sq::parse_json(const string& path)
{
    std::ifstream src(path);

    if (src.is_open() == false)
    {
        log_warning("Couldn't open json '%s'", path);
        return {};
    }

    try { return JsonValue::parse(src); }
    catch (const std::exception& e)
    {
        log_warning("problem loading '%s'\n  what():  %s", path, e.what());
    }

    return {};
}
