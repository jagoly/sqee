#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/maths.hpp>

namespace sq {

class LightPoint : NonCopyable {
public:
    LightPoint();
    void update();
    vec3 position, colour;
    float intensity;
    unique_ptr<Uniformbuffer> ubo;
    array<mat4, 6> matArr;
};


class ShadowPoint : NonCopyable {
public:
    ShadowPoint();
    void update();
    uint texSize;
    vec3 position, colour;
    float intensity;
    unique_ptr<Uniformbuffer> ubo;
    array<mat4, 6> matArr;

    void resize_texture(uint _power);
    void filter_texture(bool _enable);

    unique_ptr<TextureCube> tex;
    array<unique_ptr<Framebuffer>, 6> fboArr;
};

}
