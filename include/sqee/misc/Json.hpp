// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/macros.hpp>
#include <sqee/helpers.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Random.hpp>
#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>
#include <sqee/misc/TinyString.hpp>

#include <sqee/redist/tinyformat.hpp>
#include <sqee/redist/json.hpp>

namespace sq {

//============================================================================//

// the primary purpose for this is to use float instead of double
using JsonValue = nlohmann::basic_json<std::map, std::vector, std::string, bool, int32_t, uint32_t, float>;

namespace builtins { using sq::JsonValue; }

//============================================================================//

SQEE_API JsonValue parse_json_from_file(const String& path);

//============================================================================//

namespace maths {

template <int Size, class Type>
void from_json(const JsonValue& j, Vector<Size, Type>& vec)
{
    if (j.is_array() == false || j.size() != Size)
        throw std::invalid_argument(tfm::format("from_json: %s -> %s", j.dump(), traits::TypeName<Vector<Size, Type>>));
    for (int i = 0; i < Size; ++i) j[i].get_to(vec[i]);
}

template <int Size, class Type>
void to_json(JsonValue& j, const Vector<Size, Type>& vec)
{
    if constexpr (Size == 2) j = { vec.x, vec.y };
    if constexpr (Size == 3) j = { vec.x, vec.y, vec.z };
    if constexpr (Size == 4) j = { vec.x, vec.y, vec.z, vec.w };
}

template <class Type>
void from_json(const JsonValue& j, RandomRange<Type>& rr)
{
    if (j.is_array() == false || j.size() != 2)
        throw std::invalid_argument(tfm::format("from_json: %s -> RandomRange<%s>", j.dump(), traits::TypeName<Type>));
    j[0].get_to(rr.min); j[1].get_to(rr.max);
}

template <class Type>
void to_json(JsonValue& j, const RandomRange<Type>& rr)
{
    j = { rr.min, rr.max };
}

} // namespace maths

//============================================================================//

template <size_t Capacity>
void from_json(const JsonValue& j, FixedCapacityString<Capacity>& str)
{
    if (j.is_string() == false || j.size() > TinyString::capacity())
        throw std::invalid_argument(tfm::format("from_json: %s -> FixedCapacityString<%d>", j.dump(), Capacity));
    str = j.get_ref<const String&>();
}

template <size_t Capacity>
void to_json(JsonValue& j, const FixedCapacityString<Capacity>& str)
{
    j = String(str);
}

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
        sq::enum_from_string(j.get_ref<const std::string&>(), e); \
    } \
};
