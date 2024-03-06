#pragma once

#include <cstdint> // IWYU pragma: export
#include <sqee/redist/expected.hpp> // IWYU pragma: export
#include <optional> // IWYU pragma: export
#include <string> // IWYU pragma: export
#include <string_view> // IWYU pragma: export

//==============================================================================

typedef unsigned char uchar;
typedef unsigned int uint;

namespace sq { //###############################################################

// template <class Type> using Optional = std::optional<Type>;
// template <class Type, class Error> using Expected = tl::expected<Type, Error>;

using String = std::string;
using StringView = std::string_view;

template <size_t Capacity> class StackString;
using TinyString = StackString<15u>;
using SmallString = StackString<23u>;

template <class Type, size_t Capacity> class StackVector;

namespace maths { template <int Size, class Type> struct Vector; }
using Vec2I = maths::Vector<2, int>;
using Vec3I = maths::Vector<3, int>;
using Vec4I = maths::Vector<4, int>;
using Vec2U = maths::Vector<2, uint>;
using Vec3U = maths::Vector<3, uint>;
using Vec4U = maths::Vector<4, uint>;
using Vec2F = maths::Vector<2, float>;
using Vec3F = maths::Vector<3, float>;
using Vec4F = maths::Vector<4, float>;

namespace maths { template <int Width, int Height, class Type> struct Matrix; }
using Mat3F = maths::Matrix<3, 3, float>;
using Mat34F = maths::Matrix<3, 4, float>;
using Mat4F = maths::Matrix<4, 4, float>;

namespace maths { template <class Type> struct Quaternion; }
using QuatF = maths::Quaternion<float>;

namespace maths { template <class Type> struct RandomRange; }

class JsonAny; class JsonArray; class JsonObject; class JsonDocument;
class JsonMutAny; class JsonMutArray; class JsonMutObject; class JsonMutDocument;

} // namespace sq ##############################################################

namespace sq::coretypes { //####################################################

// using sq::Optional, sq::Expected;

using sq::String, sq::StringView;

using sq::StackString, sq::TinyString, sq::SmallString;

using sq::StackVector;

using sq::Vec2I, sq::Vec3I, sq::Vec4I;
using sq::Vec2U, sq::Vec3U, sq::Vec4U;
using sq::Vec2F, sq::Vec3F, sq::Vec4F;

using sq::Mat3F, sq::Mat34F, sq::Mat4F;

using sq::QuatF;

using sq::JsonAny, sq::JsonArray, sq::JsonObject, sq::JsonDocument;
using sq::JsonMutAny, sq::JsonMutArray, sq::JsonMutObject, sq::JsonMutDocument;

} // namespace sq::coretypes ###################################################
