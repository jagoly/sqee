#pragma once

#include <string>

#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

namespace sqe {

std::string get_shader_error(const GLuint& shader);

bool create_shader(std::string vertPath, std::string fragPath, GLuint& prog);

bool load_mesh(std::string filePath, GLuint& vao, int& pointCount);

class Camera {
public:
    Camera(sf::Vector3f _pos, float _xRot, float _yRot,
           float _width, float _height, float yFov, float zNear, float zFar);
    Camera();

    glm::mat4 viewMat;
    glm::mat4 projMat;

    glm::vec3 pos;

    float xRot;
    float yRot;
    float width;
    float height;
    float yFov;
    float zNear;
    float zFar;

    void update_viewMatrix();
    void update_projMatrix();
};

}
