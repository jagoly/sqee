#pragma once

#include <string>

#include <gl/gl_ext_3_3.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

namespace sq {

glm::vec3 get_tangent(glm::vec3 normal);

struct Camera {
    void init(glm::vec3 _pos, float _xRot, float _yRot,
              float _width, float _height, float yFov, float zNear, float zFar);

    glm::mat4 projMat;
    glm::mat4 viewMat;
    glm::mat4 projViewMat;

    glm::vec3 pos;
    glm::vec2 projAB;

    float xRot;
    float yRot;
    float width;
    float height;
    float yFov;
    float zNear;
    float zFar;

    void update_viewMat();
    void update_projMat();
    void update_projViewMat();
};

struct ScreenQuad {
    ScreenQuad();
    void draw();
    GLuint vao;
};

}
