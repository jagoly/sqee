#pragma once
#include <sqee/forward.hpp>

namespace sq {

/// A simple SQEE Camera class
class Camera : NonCopyable {
public:
    virtual ~Camera();
    Camera(GLuint _bind);
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

    const GLuint binding;
    unique_ptr<UniformBuffer> ubo;
};

}
