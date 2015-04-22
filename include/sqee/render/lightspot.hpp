#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/maths.hpp>

namespace sq {

class LightSpot : NonCopyable {
public:
    LightSpot(bool _shadow);
    void update();

    void resize_texture(uint _power);
    void filter_texture(bool _enable);

    vec3 position;
    vec3 direction;
    vec3 colour;
    float angle;
    float intensity;
    float softness;

    uint texSize;
    mat4 matrix;
    Frustum frus;

    unique_ptr<Uniformbuffer> ubo;
    unique_ptr<Texture2D> tex;
    unique_ptr<Framebuffer> fbo;

    const bool shadow;
};

}
