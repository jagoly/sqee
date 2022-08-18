// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/EnumHelper.hpp>
#include <sqee/core/Types.hpp>

// sqee doesn't use iostreams anywhere
#define JSON_NO_IO 1

// implicit conversions don't play nice with fmtlib
#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include <sqee/redist/json.hpp> // IWYU pragma: export

namespace sq {

//============================================================================//

SQEE_API JsonValue parse_json_from_file(const String& path);

SQEE_API std::optional<JsonValue> try_parse_json_from_file(const String& path);

//============================================================================//

template <size_t Capacity>
void from_json(const JsonValue& j, StackString<Capacity>& str)
{
    if (j.is_string() == false || j.size() > Capacity)
        SQEE_THROW("from_json: {} -> {}", j.dump(), type_to_string(StackString<Capacity>()));
    str = j.get_ref<const String&>();
}

template <size_t Capacity>
void to_json(JsonValue& j, const StackString<Capacity>& str)
{
    j = String(str);
}

//============================================================================//

namespace maths {

//====== Forward Declarations ================================================//

template <class T> struct RandomRange;

//============================================================================//

template <int Size, class Type>
inline void from_json(const JsonValue& json, Vector<Size, Type>& vec)
{
    if (json.is_array() == false || json.size() != Size)
        SQEE_THROW("from_json: {} -> {}", json.dump(), type_to_string(Vector<Size, Type>()));

    json[0].get_to(vec.x);
    json[1].get_to(vec.y);
    if constexpr (Size >= 3) json[2].get_to(vec.z);
    if constexpr (Size == 4) json[3].get_to(vec.w);
}

template <int Size, class Type>
inline void to_json(JsonValue& json, const Vector<Size, Type>& vec)
{
    if constexpr (Size == 2) json = { vec.x, vec.y };
    if constexpr (Size == 3) json = { vec.x, vec.y, vec.z };
    if constexpr (Size == 4) json = { vec.x, vec.y, vec.z, vec.w };
}

//----------------------------------------------------------------------------//

template <class Type>
inline void from_json(const JsonValue& json, Quaternion<Type>& quat)
{
    if (json.is_array() == false || json.size() != 4)
        SQEE_THROW("from_json: {} -> {}", json.dump(), type_to_string(Quaternion<Type>()));

    json[0].get_to(quat.x);
    json[1].get_to(quat.y);
    json[2].get_to(quat.z);
    json[3].get_to(quat.w);
}

template <class Type>
inline void to_json(JsonValue& json, const Quaternion<Type>& quat)
{
    json = { quat.x, quat.y, quat.z, quat.w };
}

//----------------------------------------------------------------------------//

template <class Type>
inline void from_json(const JsonValue& json, RandomRange<Type>& range)
{
    if (json.is_array() == false || json.size() != 2u)
        SQEE_THROW("from_json: {} -> {}", json.dump(), type_to_string(RandomRange<Type>()));

    json[0].get_to(range.min);
    json[1].get_to(range.max);
}

template <class Type>
inline void to_json(JsonValue& json, const RandomRange<Type>& range)
{
    json = { range.min, range.max };
}

//============================================================================//

}} // namespace sq::maths

//============================================================================//

template <class EnumT>
struct nlohmann::adl_serializer<EnumT, std::enable_if_t<sq::has_enum_traits_v<EnumT>>>
{
    static void to_json(sq::JsonValue& json, const EnumT& e)
    {
        json = sq::enum_to_string(e);
    }
    static void from_json(const sq::JsonValue& json, EnumT& e)
    {
        e = sq::enum_from_string<EnumT>(json.get_ref<const std::string&>());
    }
};

//============================================================================//

template<> struct fmt::formatter<sq::JsonValue> : fmt::formatter<std::string>
{
    template <class FormatContext>
    auto format(const sq::JsonValue& json, FormatContext& ctx)
    {
        return formatter<std::string>::format(impl_dump(json), ctx);
    }

    SQEE_API std::string impl_dump(const sq::JsonValue& json);
};
