#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/maths.hpp>

namespace sq {

class LightSpot : NonCopyable {
public:
    LightSpot();
    void update();
    vec3 position, direction, colour;
    float angle, intensity, softness;
    unique_ptr<Uniformbuffer> ubo;
    mat4 matrix;
};

class ShadowSpot : NonCopyable {
public:
    ShadowSpot();
    void update();
    uint texSize;
    vec3 position, direction, colour;
    float angle, intensity, softness;
    unique_ptr<Uniformbuffer> ubo;
    mat4 matrix;

    void resize_texture(uint _power);
    void filter_texture(bool _enable);

    unique_ptr<Texture2D> tex;
    unique_ptr<Framebuffer> fbo;
};

}
