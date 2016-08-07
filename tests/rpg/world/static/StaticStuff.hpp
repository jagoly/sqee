#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sqt {
namespace world {

class StaticBody final : sq::NonCopyable {
public:
    StaticBody() = default;
};

class StaticModel final : sq::NonCopyable {
public:
    StaticModel() = default;

    Vec3F PROP_position = {0.f, 0.f, 0.f};
    QuatF PROP_rotation = {0.f, 0.f, 0.f, 1.f};
    Vec3F PROP_scale    = {1.f, 1.f, 1.f};

    vector<string> PROP_mtrls = {};
    string PROP_mesh = "";
};

class StaticDecal final : sq::NonCopyable {
public:
    StaticDecal() = default;

    Vec3F PROP_position = {0.f, 0.f, 0.f};
    QuatF PROP_rotation = {0.f, 0.f, 0.f, 1.f};
    Vec3F PROP_scale    = {1.f, 1.f, 1.f};

    string PROP_material = "";
    float PROP_alpha = 1.f;
};

//class StaticSpotLight final : sq::NonCopyable {
//public:
//    StaticSpotLight() = default;

//    Vec3F PROP_position = {0.f, 0.f, 0.f};
//    QuatF PROP_rotation = {0.f, 0.f, 0.f, 1.f};
//    Vec3F PROP_colour   = {1.f, 1.f, 1.f};

//    float PROP_intensity = 1.f;
//    float PROP_softness = 0.5f;
//    float PROP_angle = 0.125f;
//    uint PROP_resolution = 0u;
//};

//class StaticPointLight final : sq::NonCopyable {
//public:
//    StaticPointLight() = default;

//    Vec3F PROP_position = {0.f, 0.f, 0.f};
//    QuatF PROP_rotation = {0.f, 0.f, 0.f, 1.f};
//    Vec3F PROP_colour   = {1.f, 1.f, 1.f};

//    float PROP_intensity = 1.f;
//    uint PROP_resolution = 0u;
//};

}}
