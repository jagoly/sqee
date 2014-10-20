#pragma once

#include <gl/gl.hpp>

namespace sq {

class Camera {
public:
    Camera();

    void init(glm::vec3 _pos, glm::vec3 _rot, float _width, float _height,
              float yFov, float zNear, float zFar);

    glm::mat4 projMat, viewMat;
    glm::vec3 pos, rot;
    float width, height;
    float yFov;
    glm::vec2 zRange;

    GLuint ubo, ubi;

    void update_viewMat();
    void update_projMat();
    void update_ubo();
};

}
