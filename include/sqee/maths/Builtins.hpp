// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sq {

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

namespace builtins {

using sq::Vec2I, sq::Vec2U, sq::Vec2F, sq::Vec3I, sq::Vec3U, sq::Vec3F, sq::Vec4I, sq::Vec4U, sq::Vec4F;
using sq::Mat3F, sq::Mat34F, sq::Mat4F, sq::QuatF;

}} // namespace sq::builtins

#ifdef SQEE_GLOBAL_BUILTINS
using namespace sq::builtins;
#endif
