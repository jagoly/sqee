#include "scenegame.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "helpers.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;
}

void SceneGame::update() {
    int X = 0;
    int Y = 0;
    int Z = 0;
    int NS = 0;
    int EW = 0;
    int FB = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) X += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) X -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) Y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) Y -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) Z += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) Z -= 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) EW += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) EW -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) NS += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) NS -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp)) FB += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown)) FB -= 1;

    cube.rot[0] += X * 0.01f;
    cube.rot[1] += Y * 0.01f;
    cube.rot[2] += Z * 0.01f;

    cube.pos[0] += EW * 0.01f;
    cube.pos[1] += NS * 0.01f;
    cube.pos[2] += FB * 0.01f;

    cube.transformMatrix = glm::mat4();
    cube.transformMatrix = glm::translate(cube.transformMatrix, glm::vec3(cube.pos[0], cube.pos[1], cube.pos[2]));
    cube.transformMatrix = glm::rotate(cube.transformMatrix, cube.rot[0], glm::vec3(1, 0, 0));
    cube.transformMatrix = glm::rotate(cube.transformMatrix, cube.rot[1], glm::vec3(0, 1, 0));
    cube.transformMatrix = glm::rotate(cube.transformMatrix, cube.rot[2], glm::vec3(0, 0, 1));
}

void SceneGame::render(sf::RenderTarget& target, float) {
    static bool first = true;

    static GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    static GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    static GLuint prog = glCreateProgram();

    static GLuint u_projMatrix, u_viewMatrix, u_transformMatrix,
                  u_lightWorldPos, u_lightSpec, u_lightDiff, u_lightAmbi,
                  u_reflSpec, u_reflDiff, u_reflAmbi,
                  u_basicTex;

    static GLuint texture = 0;
    static int pointCount;
    static GLuint monkeyVao = 0;

    if (first) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable (GL_CULL_FACE); // cull face
        glCullFace (GL_BACK); // cull back face
        glFrontFace (GL_CCW); // set counter-clock-wise vertex order to mean the front

        sqe::load_mesh("res/models/monkey.dae", monkeyVao, pointCount);
        std::cout << pointCount << std::endl;

        sf::Image image;
        image.loadFromFile("res/dice.png");
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, image.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        sqe::create_shader("res/shaders/cube_vs.glsl", "res/shaders/cube_fs.glsl", prog);

        u_projMatrix = glGetUniformLocation(prog, "projMatrix");
        u_viewMatrix = glGetUniformLocation(prog, "viewMatrix");
        u_transformMatrix = glGetUniformLocation(prog, "transformMatrix");
        u_lightWorldPos = glGetUniformLocation(prog, "lightWorldPos");
        u_lightSpec = glGetUniformLocation(prog, "lightSpec");
        u_lightDiff = glGetUniformLocation(prog, "lightDiff");
        u_lightAmbi = glGetUniformLocation(prog, "lightAmbi");

        first = false;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glUseProgram(prog);

    glUniformMatrix4fv(u_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMatrix));
    glUniformMatrix4fv(u_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix));
    glUniformMatrix4fv(u_transformMatrix, 1, GL_FALSE, glm::value_ptr(cube.transformMatrix));

    glUniform3fv(u_lightSpec, 1, glm::value_ptr(light.lightSpec));
    glUniform3fv(u_lightDiff, 1, glm::value_ptr(light.lightDiff));
    glUniform3fv(u_lightAmbi, 1, glm::value_ptr(light.lightAmbi));

    glUniform1i(u_basicTex, 0);

    glBindVertexArray(monkeyVao);
    glDrawArrays(GL_TRIANGLES, 0, pointCount);

    glUseProgram(0);
    glBindVertexArray(0);
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}

}
