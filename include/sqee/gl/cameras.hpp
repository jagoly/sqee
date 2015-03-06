#pragma once
#include "forward.hpp"

#include <vector>

#include "gl/maths.hpp"
#include "gl/uniformbuffers.hpp"

namespace sq {

class Camera : NonCopyable {
public:
    Camera(bool _useUBO);

    glm::vec3 pos, dir;
    glm::vec2 range;

    glm::mat4 projMat, viewMat;

    glm::vec2 size;
    float aspect, fov;

    std::vector<std::pair<float, Frustum>> csmVecA;
    std::vector<std::pair<float, Frustum>> csmVecB;

    virtual void update();
    virtual void recalc_frustums();

private:
    const bool useUBO;
    UniformBuffer ubo;
};


}
