#pragma once


#include <libsqee/scene.hpp>
#include <libsqee/handler.hpp>
#include <libsqee/extra/gl.hpp>

namespace sqt {

class Cube {
public:
    Cube();

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

class Light {
public:
    glm::vec3 worldPos = {2.f, 2.f, 2.f};
    glm::vec3 lightSpec = {1.f, 1.f, 1.f};
    glm::vec3 lightDiff = {0.7f, 0.7f, 0.7f};
    glm::vec3 lightAmbi = {0.2f, 0.2f, 0.2f};
};

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application* _app);

    Cube cube;
    sqe::Camera camera;
    Light light;

    void render(sf::RenderTarget& target, float ft);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

}
