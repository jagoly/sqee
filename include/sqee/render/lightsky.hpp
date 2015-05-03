#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/maths.hpp>

namespace sq {

class LightSky : NonCopyable {
public:
    LightSky();
    void update();
    vec3 colour, direction;
    unique_ptr<Uniformbuffer> ubo;
};

class ShadowSky : NonCopyable {
public:
    ShadowSky();
    void update();
    uint texSize;
    vec3 colour, direction;
    unique_ptr<Uniformbuffer> ubo;

    void resize_texture(uint _power);
    void filter_texture(bool _enable);

    array<mat4, 4> matArr;
    array<float, 4> splitArr;
    array<Frustum, 4> csmfrArr;
    unique_ptr<Texture2DArray> tex;
    array<unique_ptr<Framebuffer>, 4> fboArr;
    const Camera* camera = nullptr;
};

}
