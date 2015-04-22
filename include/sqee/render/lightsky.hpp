#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/maths.hpp>

namespace sq {

class LightSky : NonCopyable {
public:
    LightSky(bool _shadow);
    void update();

    void resize_texture(uint _power);
    void filter_texture(bool _enable);

    vec3 colour;
    vec3 direction;

    uint texSize;
    array<mat4, 4> matArr;
    array<Frustum, 4> frusArr;
    Camera* camera = nullptr;

    array<float, 4> splitArr;
    array<Frustum, 4> csmfrArr;
    unique_ptr<Uniformbuffer> ubo;
    unique_ptr<Texture2DArray> tex;
    array<unique_ptr<Framebuffer>, 4> fboArr;

    const bool shadow;
};

}
