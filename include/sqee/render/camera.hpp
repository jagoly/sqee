#pragma once
#include <sqee/forward.hpp>

#include <sqee/maths/culling.hpp>

namespace sq {

class Camera : NonCopyable {
public:
    virtual ~Camera();
    Camera(GLuint _bind);

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
    unique_ptr<Uniformbuffer> ubo;
    virtual void update();
};

}
