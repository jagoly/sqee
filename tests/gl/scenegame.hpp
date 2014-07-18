#pragma once

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>

#include <libsqee/base.hpp>

namespace sqt {

class Cube {
public:
    Cube();

    glm::mat4 transformMatrix;
    float rot[3] = {0.f, 0.f, 0.f};
    float pos[3] = {0.f, 0.f, 0.f};

    GLuint vao, prog;
    GLuint u_transformMatrix;
private:
    GLfloat pointsCube[108] = {
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

    GLfloat coloursCube[108] = {
        0.f, 0.f, 1.f, // Far
        0.f, 0.f, 1.f, // Blue
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,

        0.f, 1.f, 0.f, // Near
        0.f, 1.f, 0.f, // Green
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,

        1.f, 0.f, 0.f, // Top
        1.f, 0.f, 0.f, // Red
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,

        0.f, 1.f, 1.f, // Right
        0.f, 1.f, 1.f, // Cyan
        0.f, 1.f, 1.f,
        0.f, 1.f, 1.f,
        0.f, 1.f, 1.f,
        0.f, 1.f, 1.f,

        1.f, 0.f, 1.f, // Bottom
        1.f, 0.f, 1.f, // Magenta
        1.f, 0.f, 1.f,
        1.f, 0.f, 1.f,
        1.f, 0.f, 1.f,
        1.f, 0.f, 1.f,

        1.f, 1.f, 0.f, // Left
        1.f, 1.f, 0.f, // Yellow
        1.f, 1.f, 0.f,
        1.f, 1.f, 0.f,
        1.f, 1.f, 0.f,
        1.f, 1.f, 0.f
    };

    GLuint vert, frag, vboPoints, vboColours;
};

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application*);


    Cube cube;

    void render(sf::RenderTarget&, float);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

}
