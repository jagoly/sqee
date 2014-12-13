#pragma once

#include <gl/gl.hpp>
#include <maths/glm.hpp>
#include <maths/physics.hpp>
#include <gl/uniformbuffers.hpp>

namespace sq {

class BaseCamera : NonCopyable {
public:
    BaseCamera(bool _useUBO);

    glm::mat4 projMat, viewMat;
    glm::vec3 pos, dir;
    glm::vec2 range;

    virtual void update();

private:
    const bool useUBO;
    UniformBuffer ubo;
};

class LookatCamera : public BaseCamera {
public:
    using BaseCamera::BaseCamera;

    glm::vec2 size;
    float strictAspect;
    float fov;

    void update();
};

}
