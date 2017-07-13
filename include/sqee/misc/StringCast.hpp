#pragma once

#include <string>
#include <ostream>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sq {

//============================================================================//

std::string chai_string(const Vec2I& arg);
std::string chai_string(const Vec2U& arg);
std::string chai_string(const Vec2F& arg);

std::string glsl_string(const Vec2I& arg);
std::string glsl_string(const Vec2U& arg);
std::string glsl_string(const Vec2F& arg);

std::string chai_string(const Vec3I& arg);
std::string chai_string(const Vec3U& arg);
std::string chai_string(const Vec3F& arg);

std::string glsl_string(const Vec3I& arg);
std::string glsl_string(const Vec3U& arg);
std::string glsl_string(const Vec3F& arg);

std::string chai_string(const Vec4I& arg);
std::string chai_string(const Vec4U& arg);
std::string chai_string(const Vec4F& arg);

std::string glsl_string(const Vec4I& arg);
std::string glsl_string(const Vec4U& arg);
std::string glsl_string(const Vec4F& arg);

std::string chai_string(const Mat3F& arg);
std::string chai_string(const Mat34F& arg);
std::string chai_string(const Mat4F& arg);

std::string chai_string(const QuatF& arg);
std::string glsl_string(const QuatF& arg);

//============================================================================//

inline std::ostream& operator<<(std::ostream& os, const Vec2I& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec2U& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec2F& arg) { return os << sq::chai_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const Vec3I& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec3U& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec3F& arg) { return os << sq::chai_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const Vec4I& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec4U& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec4F& arg) { return os << sq::chai_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const Mat3F& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Mat34F& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Mat4F& arg) { return os << sq::chai_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const QuatF& arg) { return os << sq::chai_string(arg); }

//============================================================================//

} // namespace sq
