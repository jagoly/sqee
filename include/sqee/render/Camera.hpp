#pragma once

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

namespace sq {

/// A simple SQEE Camera class
class Camera : NonCopyable {
public:
    Camera();
    virtual ~Camera() = default;

    virtual void update();

    Vec3F pos;
    Vec3F dir;
    float rmin;
    float rmax;
    Vec2F size;
    float fov;

    Mat4F projMat;
    Mat4F viewMat;
    //Frustum frus;

    UniformBuffer ubo;
};

}
