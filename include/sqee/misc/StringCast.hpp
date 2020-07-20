#pragma once

#include <ostream>

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

namespace sq {

//============================================================================//

SQEE_API String to_string(const Vec2I& arg);
SQEE_API String to_string(const Vec2U& arg);
SQEE_API String to_string(const Vec2F& arg);

SQEE_API String glsl_string(const Vec2I& arg);
SQEE_API String glsl_string(const Vec2U& arg);
SQEE_API String glsl_string(const Vec2F& arg);

SQEE_API String to_string(const Vec3I& arg);
SQEE_API String to_string(const Vec3U& arg);
SQEE_API String to_string(const Vec3F& arg);

SQEE_API String glsl_string(const Vec3I& arg);
SQEE_API String glsl_string(const Vec3U& arg);
SQEE_API String glsl_string(const Vec3F& arg);

SQEE_API String to_string(const Vec4I& arg);
SQEE_API String to_string(const Vec4U& arg);
SQEE_API String to_string(const Vec4F& arg);

SQEE_API String glsl_string(const Vec4I& arg);
SQEE_API String glsl_string(const Vec4U& arg);
SQEE_API String glsl_string(const Vec4F& arg);

SQEE_API String to_string(const Mat3F& arg);
SQEE_API String to_string(const Mat34F& arg);
SQEE_API String to_string(const Mat4F& arg);

SQEE_API String to_string(const QuatF& arg);

SQEE_API String glsl_string(const QuatF& arg);

//============================================================================//

namespace maths {

inline std::ostream& operator<<(std::ostream& os, const Vec2I& arg) { return os << to_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec2U& arg) { return os << to_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec2F& arg) { return os << to_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const Vec3I& arg) { return os << to_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec3U& arg) { return os << to_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec3F& arg) { return os << to_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const Vec4I& arg) { return os << to_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec4U& arg) { return os << to_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec4F& arg) { return os << to_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const Mat3F& arg) { return os << to_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Mat34F& arg) { return os << to_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Mat4F& arg) { return os << to_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const QuatF& arg) { return os << to_string(arg); }

} // namespace maths

//============================================================================//

} // namespace sq
