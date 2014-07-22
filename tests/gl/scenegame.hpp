#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/scene.hpp>
#include <libsqee/handler.hpp>
#include <libsqee/extra/gl.hpp>

namespace sqt {

class Cube {
public:
    Cube();

    glm::mat4 transformMatrix;
    float rot[3] = {0.f, 0.f, 0.f};
    float pos[3] = {0.f, 0.f, 0.f};

    glm::vec3 reflSpec = {1.f, 1.f, 1.f};
    glm::vec3 reflDiff = {1.f, 0.5f, 0.f};
    glm::vec3 reflAmbi = {1.f, 1.f, 1.f};

    float specExp = 100.f;

    GLuint vao;
private:
    GLfloat points[108] = {
        -0.5f, -0.5f, 0.5f, // Far
        -0.5f,  0.5f, 0.5f,
         0.5f,  0.5f, 0.5f,
         0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
         0.5f,  0.5f, 0.5f,

        -0.5f, -0.5f,  -0.5f, // Near
        -0.5f,  0.5f,  -0.5f,
         0.5f,  0.5f,  -0.5f,
         0.5f, -0.5f,  -0.5f,
        -0.5f, -0.5f,  -0.5f,
         0.5f,  0.5f,  -0.5f,

        -0.5f,  0.5f, -0.5f, // Top
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,

         0.5f, -0.5f, -0.5f, // Right
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f, // Bottom
        -0.5f, -0.5f, 0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f, // Left
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f
    };

    GLfloat texcoords[72] = {
        0.f,    0.f, // Far
        0.f,    0.5f,
        0.334f, 0.5f,
        0.334f, 0.f,
        0.f,    0.f,
        0.334f, 0.5f,

        0.667f, 0.5f, // Near
        0.667f, 1.f,
        1.f,    1.f,
        1.f,    0.5f,
        0.667f, 0.5f,
        1.f,    1.f,

        0.667f, 0.f, // Top
        0.667f, 0.5f,
        1.f,    0.5f,
        1.f,    0.f,
        0.667f, 0.f,
        1.f,    0.5f,

        0.334f, 0.5f, // Right
        0.334f, 1.f,
        0.667f, 1.f,
        0.667f, 0.5f,
        0.334f, 0.5f,
        0.667f, 1.f,

        0.f,    0.5f, // Bottom
        0.f,    1.f,
        0.334f, 1.f,
        0.334f, 0.5f,
        0.f,    0.5f,
        0.334f, 1.f,

        0.334f, 0.f, // Left
        0.334f, 0.5f,
        0.667f, 0.5f,
        0.667f, 0.f,
        0.334f, 0.f,
        0.667f, 0.5f,
    };

    GLfloat normals[108] = {
        0.f, 0.f, 1.f, // Far
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,

        0.f, 0.f, -1.f, // Near
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,

        0.f, 1.f, 0.f, // Top
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,

        1.f, 0.f, 0.f, // Right
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,

        0.f, -1.f, 0.f, // Bottom
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, -1.f, 0.f,

        -1.f, 0.f, 0.f, // Left
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f,
        -1.f, 0.f, 0.f
    };

    GLuint vboPoints, vboTexcoords, vboNormals;
};

class Camera {
public:
    Camera();

    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;

    float pos[3] = {0.f, 0.f, 2.f};
    float yaw = 0.f;
};

class Light {
public:
    glm::vec3 worldPos = {0.f, 100.f, 10.f};
    glm::vec3 lightSpec = {1.f, 1.f, 1.f};
    glm::vec3 lightDiff = {0.7f, 0.7f, 0.7f};
    glm::vec3 lightAmbi = {0.2f, 0.2f, 0.2f};
};

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application*);

    Cube cube;
    Camera camera;
    Light light;

    void render(sf::RenderTarget&, float);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

}
