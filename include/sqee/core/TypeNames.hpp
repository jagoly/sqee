#pragma once

#include <sqee/core/Strings.hpp>
#include <sqee/core/TypeAliases.hpp>
#include <sqee/core/TypeTraits.hpp>

namespace sq { //###############################################################

template <> inline constexpr auto type_name_v<std::nullptr_t> = StackString("nullptr");

template <> inline constexpr auto type_name_v<bool> = StackString("bool");
template <> inline constexpr auto type_name_v<int8_t> = StackString("int8");
template <> inline constexpr auto type_name_v<int16_t> = StackString("int16");
template <> inline constexpr auto type_name_v<int32_t> = StackString("int32");
template <> inline constexpr auto type_name_v<int64_t> = StackString("int64");
template <> inline constexpr auto type_name_v<uint8_t> = StackString("uint8");
template <> inline constexpr auto type_name_v<uint16_t> = StackString("uint16");
template <> inline constexpr auto type_name_v<uint32_t> = StackString("uint32");
template <> inline constexpr auto type_name_v<uint64_t> = StackString("uint64");
template <> inline constexpr auto type_name_v<float> = StackString("float32");
template <> inline constexpr auto type_name_v<double> = StackString("float64");

template <> inline constexpr auto type_name_v<String> = StackString("String");
template <> inline constexpr auto type_name_v<StringView> = StackString("StringView");

template <size_t Capacity>
inline constexpr auto type_name_v<StackString<Capacity>> = format_consteval<"StackString<{}>", Capacity>();
template <> inline constexpr auto type_name_v<TinyString> = StackString("TinyString");
template <> inline constexpr auto type_name_v<SmallString> = StackString("SmallString");

template <class Type, size_t Capacity>
inline constexpr auto type_name_v<StackVector<Type, Capacity>> = format_consteval<"StackVector<{}, {}>", type_name_v<Type>, Capacity>();

template <int Size, class Type>
inline constexpr auto type_name_v<maths::Vector<Size, Type>> = format_consteval<"Vector<{}, {}>", Size, type_name_v<Type>>();
template <> inline constexpr auto type_name_v<Vec2I> = StackString("Vec2I");
template <> inline constexpr auto type_name_v<Vec3I> = StackString("Vec3I");
template <> inline constexpr auto type_name_v<Vec4I> = StackString("Vec4I");
template <> inline constexpr auto type_name_v<Vec2U> = StackString("Vec2U");
template <> inline constexpr auto type_name_v<Vec3U> = StackString("Vec3U");
template <> inline constexpr auto type_name_v<Vec4U> = StackString("Vec4U");
template <> inline constexpr auto type_name_v<Vec2F> = StackString("Vec2F");
template <> inline constexpr auto type_name_v<Vec3F> = StackString("Vec3F");
template <> inline constexpr auto type_name_v<Vec4F> = StackString("Vec4F");

template <int Width, int Height, class Type>
inline constexpr auto type_name_v<maths::Matrix<Width, Height, Type>> = format_consteval<"Matrix<{}, {}, {}>", Width, Height, type_name_v<Type>>();
template <> inline constexpr auto type_name_v<Mat3F> = StackString("Mat3F");
template <> inline constexpr auto type_name_v<Mat34F> = StackString("Mat34F");
template <> inline constexpr auto type_name_v<Mat4F> = StackString("Mat4F");

template <class Type>
inline constexpr auto type_name_v<maths::Quaternion<Type>> = format_consteval<"Quaternion<{}>", type_name_v<Type>>();
template <> inline constexpr auto type_name_v<maths::Quaternion<float>> = StackString("QuatF");

template <class Type>
inline constexpr auto type_name_v<maths::RandomRange<Type>> = format_consteval<"RandomRange<{}>", type_name_v<Type>>();

template <> inline constexpr auto type_name_v<JsonAny> = StackString("JsonAny");
template <> inline constexpr auto type_name_v<JsonArray> = StackString("JsonArray");
template <> inline constexpr auto type_name_v<JsonObject> = StackString("JsonObject");
template <> inline constexpr auto type_name_v<JsonDocument> = StackString("JsonDocument");

template <> inline constexpr auto type_name_v<JsonMutAny> = StackString("JsonMutAny");
template <> inline constexpr auto type_name_v<JsonMutArray> = StackString("JsonMutArray");
template <> inline constexpr auto type_name_v<JsonMutObject> = StackString("JsonMutObject");
template <> inline constexpr auto type_name_v<JsonMutDocument> = StackString("JsonMutDocument");

} // namespace sq ##############################################################
