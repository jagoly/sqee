#pragma once
#include <sqee/forward.hpp>

#include <vector>

#include <sqee/gl/maths.hpp>
#include <sqee/gl/uniformbuffers.hpp>

#include "camera.hpp"

namespace sqt {

class SkyLight {
public:
    SkyLight();

    void update();
    void bind_ubo();

    vec3 colour;
    vec3 direction;
    array<mat4, 4> matArr;
    sq::UniformBuffer ubo;

    MainCamera* camera = nullptr;
};

class SpotLight {
public:
    SpotLight();

    void update();
    void bind_ubo();

    uint baseSize = 256u;
    vec3 position;
    float angle;
    vec3 direction;
    float intensity;
    vec3 colour;
    float softness;
    mat4 matrix;
    sq::UniformBuffer ubo;

    MainCamera* camera = nullptr;
};

class PointLight {
public:
    void update();
    void render(float _ft);

    std::vector<vec2> shape;
};

}
