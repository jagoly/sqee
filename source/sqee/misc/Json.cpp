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
        log_error("Couldn't open json '%s'", path);
    }

    return JsonValue::parse(src);
}