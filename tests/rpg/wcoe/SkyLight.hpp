#pragma once

#include <sqee/gl/Textures.hpp>
#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

namespace sqt {

class World;

class SkyLight final : sq::NonCopyable {
public:
    SkyLight();

    void configure(const World& _world);
    void update(const World& _world);

    // User Properties /////
    Vec3F PROP_direction = {0.f, 0.f, -1.f};
    Vec3F PROP_colour    = {1.f, 1.f, 1.f};
    float PROP_density   = 0.25f;

    // System Properties /////
    sq::UniformBuffer ubo;
    sq::Texture2DArray texA;
    sq::Texture2DArray texB;
    array<sq::OrthoFrus, 4> orthArrA;
    array<sq::OrthoFrus, 2> orthArrB;
    array<Mat4F, 4> matArrA;
    array<Mat4F, 2> matArrB;
    Vec4F splits;
};

}
