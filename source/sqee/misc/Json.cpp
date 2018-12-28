#include <sqee/misc/Json.hpp>

#include <sqee/debug/Logging.hpp>

#include <fstream>

using namespace sq;

//============================================================================//

#define SQEE_FROM_JSON_INVALID_ARGUMENT(Type) \
throw std::invalid_argument(sq::build_string("from_json: ", j.dump(), " -> "#Type));

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

void maths::from_json(const JsonValue& j, Vector2<int>& vec)
{
    if (j.is_array() == false || j.size() != 2u)
        SQEE_FROM_JSON_INVALID_ARGUMENT(Vec2I)

    j[0].get_to(vec.x); j[1].get_to(vec.y);
}

void maths::from_json(const JsonValue& j, Vector3<int>& vec)
{
    if (j.is_array() == false || j.size() != 3u)
        SQEE_FROM_JSON_INVALID_ARGUMENT(Vec3I)

    j[0].get_to(vec.x); j[1].get_to(vec.y); j[2].get_to(vec.z);
}

void maths::from_json(const JsonValue& j, Vector4<int>& vec)
{
    if (j.is_array() == false || j.size() != 4u)
        SQEE_FROM_JSON_INVALID_ARGUMENT(Vec4I)

    j[0].get_to(vec.x); j[1].get_to(vec.y); j[2].get_to(vec.z); j[3].get_to(vec.w);
}

void maths::to_json(JsonValue& j, const Vector2<int>& vec) { j = { vec.x, vec.y }; }

void maths::to_json(JsonValue& j, const Vector3<int>& vec) { j = { vec.x, vec.y, vec.z }; }

void maths::to_json(JsonValue& j, const Vector4<int>& vec) { j = { vec.x, vec.y, vec.z, vec.w }; }

void maths::from_json(const JsonValue& j, Vector2<float>& vec)
{
    if (j.is_array() == false || j.size() != 2u)
        SQEE_FROM_JSON_INVALID_ARGUMENT(Vec2F)

    j[0].get_to(vec.x); j[1].get_to(vec.y);
}

void maths::from_json(const JsonValue& j, Vector3<float>& vec)
{
    if (j.is_array() == false || j.size() != 3u)
        SQEE_FROM_JSON_INVALID_ARGUMENT(Vec3F)

    j[0].get_to(vec.x); j[1].get_to(vec.y); j[2].get_to(vec.z);
}

void maths::from_json(const JsonValue& j, Vector4<float>& vec)
{
    if (j.is_array() == false || j.size() != 4u)
        SQEE_FROM_JSON_INVALID_ARGUMENT(Vec4F)

    j[0].get_to(vec.x); j[1].get_to(vec.y); j[2].get_to(vec.z); j[3].get_to(vec.w);
}

void maths::to_json(JsonValue& j, const Vector2<float>& vec) { j = { vec.x, vec.y }; }

void maths::to_json(JsonValue& j, const Vector3<float>& vec) { j = { vec.x, vec.y, vec.z }; }

void maths::to_json(JsonValue& j, const Vector4<float>& vec) { j = { vec.x, vec.y, vec.z, vec.w }; }

//============================================================================//

void sq::from_json(const JsonValue& j, TinyString& str)
{
    if (j.is_string() == false || j.size() > TinyString::capacity())
        SQEE_FROM_JSON_INVALID_ARGUMENT(TinyString)

    str = j.get_ref<const String&>();
}

void sq::to_json(JsonValue& j, const TinyString& str)
{
    j = String(str);
}

//============================================================================//

#undef SQEE_FROM_JSON_INVALID_ARGUMENT
