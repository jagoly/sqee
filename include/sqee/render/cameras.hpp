#pragma once
#include <sqee/forward.hpp>

#include <sqee/maths/culling.hpp>

namespace sq {

class Camera : NonCopyable {
public:
    Camera();

    vec3 pos, dir;
    vec2 range;
    vec2 size;
    float fov;

    mat4 projMat;
    mat4 viewMat;
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
