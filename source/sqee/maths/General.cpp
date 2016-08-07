#include <sqee/maths/General.hpp>

namespace sq { namespace maths {

Vec3F make_tangent(Vec3F _normal) {

    const Vec3F tX = cross(_normal, Vec3F(1.f, 0.f, 0.f));
    const Vec3F tY = cross(_normal, Vec3F(0.f, 1.f, 0.f));
    const Vec3F tZ = cross(_normal, Vec3F(0.f, 0.f, 1.f));

    const float lX = length(tX);
    const float lY = length(tY);
    const float lZ = length(tZ);

    const float maxLength = max(max(lX, lY), lZ);

    if (maxLength == lX) return normalize(tX);
    if (maxLength == lY) return normalize(tY);
    if (maxLength == lZ) return normalize(tZ);
}

Vec3F norm_from_tri(Vec3F _a, Vec3F _b, Vec3F _c) {
    return normalize(cross(_b - _a, _c - _a));
}

}}
