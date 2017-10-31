#include <fstream>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Json.hpp>

using namespace sq;

//============================================================================//

JsonValue sq::parse_json_from_file(const string& path)
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
