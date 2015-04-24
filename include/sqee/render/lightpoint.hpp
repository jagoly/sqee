#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/maths.hpp>

namespace sq {

class LightPoint : NonCopyable {
public:
    LightPoint(bool _shadow);
    void update();

    void resize_texture(uint _power);
    void filter_texture(bool _enable);

    vec3 position;
    vec3 colour;
    float intensity;

    uint texSize;
    array<mat4, 6> matArr;
    array<sq::Frustum, 6> frusArr;
    sq::Sphere sphere;

    unique_ptr<Uniformbuffer> ubo;
    unique_ptr<TextureCube> tex;
    array<unique_ptr<Framebuffer>, 6> fboArr;

    const bool shadow;
};

}
