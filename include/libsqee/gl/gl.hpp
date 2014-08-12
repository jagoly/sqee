#pragma once

#include <string>

#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

namespace sq {

glm::vec4 get_tangent(glm::vec3 normal);

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
    glm::mat4 invProjViewMat;

    glm::vec3 pos;

    float xRot;
    float yRot;
    float width;
    float height;
    float yFov;
    float zNear;
    float zFar;

    void update_viewMat();
    void update_projMat();
    void update_invProjViewMat();
};

}
