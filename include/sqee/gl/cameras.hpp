#pragma once

#include <gl/gl.hpp>
#include <gl/uniformbuffers.hpp>

namespace sq {

class Camera {
public:
    Camera();

    void init(glm::vec3 _pos, glm::vec3 _dir,
              glm::vec2 _zRange, glm::vec2 _aspect, float _fov);

    glm::mat4 projMat, viewMat;
    glm::vec3 pos, dir;
    glm::vec2 zRange;
    glm::vec2 aspect;
    float fov;

    sq::UniformBuffer ubo;

    void update_viewMat();
    void update_projMat(float _aspStnd = 0.f);
    void update_ubo();
};

}
