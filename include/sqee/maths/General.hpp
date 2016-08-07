#pragma once

#include <sqee/maths/Vectors.hpp>

namespace sq {
namespace maths {

Vec3F make_tangent(Vec3F _normal);

Vec3F norm_from_tri(Vec3F _a, Vec3F _b, Vec3F _c);

}}
