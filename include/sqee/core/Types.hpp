// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

// todo: might be good to split all of the maths functions into seperate files,
// leaving just the types and string conversions in the main headers

#include <sqee/maths/Scalar.hpp> // IWYU pragma: export
#include <sqee/maths/Vectors.hpp> // IWYU pragma: export
#include <sqee/maths/Matrices.hpp> // IWYU pragma: export
#include <sqee/maths/Quaternion.hpp> // IWYU pragma: export

#include <sqee/misc/StackString.hpp> // IWYU pragma: export

#include <string> // IWYU pragma: export
#include <string_view> // IWYU pragma: export

//----------------------------------------------------------------------------//

// here we only forward declare JsonValue, to use it include misc/Json.hpp

#include <sqee/redist/json_fwd.hpp> // IWYU pragma: export

#include <map>
#include <vector>

//============================================================================//

typedef unsigned int uint;

//============================================================================//

namespace sq {

using String = std::string;
using StringView = std::string_view;

using TinyString = StackString<15u>;
using SmallString = StackString<31u>;

using Vec2I = maths::Vector<2, int>;
using Vec3I = maths::Vector<3, int>;
using Vec4I = maths::Vector<4, int>;
using Vec2U = maths::Vector<2, uint>;
using Vec3U = maths::Vector<3, uint>;
using Vec4U = maths::Vector<4, uint>;
using Vec2F = maths::Vector<2, float>;
using Vec3F = maths::Vector<3, float>;
using Vec4F = maths::Vector<4, float>;

using Mat3F = maths::Matrix<3, 3, float>;
using Mat34F = maths::Matrix<3, 4, float>;
using Mat4F = maths::Matrix<4, 4, float>;

using QuatF = maths::Quaternion<float>;

using JsonValue = nlohmann::basic_json<std::map, std::vector, std::string, bool, int32_t, uint32_t, float>;

} // namespace sq

//============================================================================//

namespace sq::coretypes {

using sq::String, sq::StringView;

using sq::TinyString, sq::SmallString;

using sq::Vec2I, sq::Vec3I, sq::Vec4I;
using sq::Vec2U, sq::Vec3U, sq::Vec4U;
using sq::Vec2F, sq::Vec3F, sq::Vec4F;

using sq::Mat3F, sq::Mat34F, sq::Mat4F;

using sq::QuatF;

using sq::JsonValue;

} // namespace sq::coretypes
