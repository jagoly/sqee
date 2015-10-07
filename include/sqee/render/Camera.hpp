#pragma once
#include <sqee/forward.hpp>

namespace sq {

/// A simple SQEE Camera class
class Camera : NonCopyable {
public:
    Camera();
    virtual ~Camera() = default;

    virtual void update();

    fvec3 pos;
    fvec3 dir;
    float rmin;
    float rmax;
    fvec2 size;
    float fov;

    fmat4 projMat;
    fmat4 viewMat;
    Frustum frus;

    unique_ptr<UniformBuffer> ubo;
};

}
