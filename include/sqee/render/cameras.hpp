#pragma once
#include <sqee/forward.hpp>

#include <sqee/maths/culling.hpp>

namespace sq {

class Camera : NonCopyable {
public:
    Camera();

    fvec3 pos;
    fvec3 dir;
    float rmin;
    float rmax;
    fvec2 size;
    float fov;

    fmat4 projMat;
    fmat4 viewMat;
    Frustum frus;

    virtual void update();
};

class UboCamera : public Camera {
public:
    UboCamera(const vector<pair<string, uint>>& _extra);

    unique_ptr<Uniformbuffer> ubo;
    GLuint binding = 0;

    void update();
};

}
