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

using JsonValue = nlohmann::json;

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

#define SQEE_FROM_JSON_INVALID_ARGUMENT(Type) \
    throw std::invalid_argument(sq::build_string("from_json: ", j.dump(), " -> "#Type));

#define SQEE_ENUM_JSON_CONVERSION_DECLARATIONS(Type) \
void from_json(const JsonValue&, Type&); \
void to_json(JsonValue&, const Type&);

#define SQEE_ENUM_JSON_CONVERSION_DEFINITIONS_INNER(Case) \
    if (ref == #Case) { e = EnumType::Case; return; }

#define SQEE_ENUM_JSON_CONVERSION_DEFINITIONS(NameSpace, Type, ...) \
void NameSpace::from_json(const JsonValue& j, Type& e) \
{ \
    SWITCH(e) { CASE(__VA_ARGS__) {} } SWITCH_END; \
    using EnumType = Type; \
    if (j.is_string() == false) \
        SQEE_FROM_JSON_INVALID_ARGUMENT(Type) \
    const String& ref = j; \
    SQEE_FOR_EACH(SQEE_ENUM_JSON_CONVERSION_DEFINITIONS_INNER, __VA_ARGS__) \
    SQEE_FROM_JSON_INVALID_ARGUMENT(Type) \
} \
void NameSpace::to_json(JsonValue& j, const Type& e) { j = enum_to_string(e); }
