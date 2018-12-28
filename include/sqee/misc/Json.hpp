// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/macros.hpp>
#include <sqee/helpers.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/misc/Builtins.hpp>
#include <sqee/misc/TinyString.hpp>

#include <sqee/redist/nl_json.hpp>

namespace sq {

//============================================================================//

// the primary purpose for this is to use float instead of double
using JsonValue = nlohmann::basic_json<std::map, std::vector, std::string, bool, int32_t, uint32_t, float>;

namespace builtins { using sq::JsonValue; }

//============================================================================//

SQEE_API JsonValue parse_json_from_file(const String& path);

//============================================================================//

namespace maths {

SQEE_API void from_json(const JsonValue&, maths::Vector2<int>&);
SQEE_API void from_json(const JsonValue&, maths::Vector3<int>&);
SQEE_API void from_json(const JsonValue&, maths::Vector4<int>&);

SQEE_API void to_json(JsonValue&, const maths::Vector2<int>&);
SQEE_API void to_json(JsonValue&, const maths::Vector3<int>&);
SQEE_API void to_json(JsonValue&, const maths::Vector4<int>&);

SQEE_API void from_json(const JsonValue&, maths::Vector2<float>&);
SQEE_API void from_json(const JsonValue&, maths::Vector3<float>&);
SQEE_API void from_json(const JsonValue&, maths::Vector4<float>&);

SQEE_API void to_json(JsonValue&, const maths::Vector2<float>&);
SQEE_API void to_json(JsonValue&, const maths::Vector3<float>&);
SQEE_API void to_json(JsonValue&, const maths::Vector4<float>&);

} // namespace maths

SQEE_API void from_json(const JsonValue&, TinyString&);

SQEE_API void to_json(JsonValue&, const TinyString&);

//============================================================================//

} // namespace sq

//============================================================================//

#define SQEE_ENUM_JSON_CONVERSIONS(Type) \
template<> struct nlohmann::adl_serializer<Type> \
{ \
    static void to_json(sq::JsonValue& j, const Type& e) \
    { \
        j = sq::enum_to_string(e); \
    } \
    static void from_json(const sq::JsonValue& j, Type& e) \
    { \
        sq::enum_from_string(j, e); \
    } \
};
