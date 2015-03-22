#pragma once
#include "forward.hpp"

#include <vector>

#include "gl/uniformbuffers.hpp"

namespace sq {

class Camera : NonCopyable {
public:
    Camera();

    vec3 pos, dir;
    vec2 range;

    mat4 projMat;
    mat4 viewMat;

    vec2 size;
    float fov;

    virtual void update();
};

class UboCamera : public Camera {
public:
    UboCamera();

    Uniformbuffer ubo;
    GLuint binding = 0;

    void update();
};


}
