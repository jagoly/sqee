#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/maths.hpp>

namespace sq {

class SkyLight : NonCopyable {
public:
    SkyLight(bool _shadow, bool _uformbuf);
    void update();

    void resize_texture(uint _power);
    void filter_texture(bool _enable);

    vec3 colour;
    vec3 direction;

    uint texSize;
    array<mat4, 4> matArr;
    array<sq::Frustum, 4> frusArr;

    array<pair<float, Frustum>, 4>* csmArr = nullptr;
    unique_ptr<Uniformbuffer> ubo;
    unique_ptr<Texture2DArray> tex;
    array<unique_ptr<Framebuffer>, 4> fboArr;

    const bool shadow;
    const bool uformbuf;
};

}
