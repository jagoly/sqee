#pragma once
#include <sqee/forward.hpp>

#include <vector>

#include <sqee/gl/maths.hpp>
#include <sqee/gl/uniformbuffers.hpp>

#include "camera.hpp"

namespace sqt {

class SkyLight : NonCopyable {
public:
    SkyLight();

    void update();
    void bind_ubo();

    vec3 colour;
    vec3 direction;
    array<mat4, 4> matArr;
    sq::Uniformbuffer ubo;

    MainCamera* camera = nullptr;
};

class SpotLight : NonCopyable {
public:
    SpotLight();

    void update();
    void bind_ubo();

    uint baseSize;
    vec3 position;
    float angle;
    vec3 direction;
    float intensity;
    vec3 colour;
    float softness;
    mat4 matrix;
    sq::Uniformbuffer ubo;

    void draw();

    MainCamera* camera = nullptr;

private:
    sq::Frustum frustum;
    std::vector<vec2> shape;
};

}
