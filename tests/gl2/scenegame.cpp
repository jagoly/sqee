#include "scenegame.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "helpers.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;

    floor.load_map("res/maps/test.json");

    camera = sqe::Camera({0.f, 1.f, 6.f}, 0.5f, 0.f, 4, 3, 1.17f, 0.1f, 100.f);
    //camera = sqe::Camera({0.f, 1.f, 10.f}, 0.f, 0.f, 4, 3, 1.17f, 0.1f, 100.f);
    camera.update_projMatrix();
    camera.update_viewMatrix();
}

void SceneGame::update() {
    int NS = 0;
    int EW = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) EW += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) EW -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) NS += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) NS -= 1;

    camera.pos.x += EW * 0.01667f;
    camera.pos.y += NS * 0.01667f;

    camera.update_projMatrix();
    camera.update_viewMatrix();
}

void SceneGame::render(sf::RenderTarget& target, float) {
    static bool first = true;

    static GLuint prog = glCreateProgram();

    static GLuint u_projMatrix, u_viewMatrix,
                  u_w_lightPos, u_lightSpec, u_lightDiff, u_lightAmbi,
                  u_basicTex;

    static GLuint texture = 0;

    static int pCount;

    static GLuint vaoFloor = 0;

    static GLuint vboPoints = 0;
    static GLuint vboNormals = 0;

    if (first) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        floor.get_models(vaoFloor, pCount);

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

        sqe::create_shader("res/shaders/main_vs.glsl", "res/shaders/main_fs.glsl", prog);

        u_projMatrix = glGetUniformLocation(prog, "projMatrix");
        u_viewMatrix = glGetUniformLocation(prog, "viewMatrix");
        u_w_lightPos = glGetUniformLocation(prog, "w_lightPos");
        u_lightSpec = glGetUniformLocation(prog, "lightSpec");
        u_lightDiff = glGetUniformLocation(prog, "lightDiff");
        u_lightAmbi = glGetUniformLocation(prog, "lightAmbi");

        first = false;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glUseProgram(prog);

    glUniformMatrix4fv(u_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
    glUniformMatrix4fv(u_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

    glUniform3fv(u_lightDiff, 1, glm::value_ptr(light.lightDiff));
    glUniform3fv(u_lightAmbi, 1, glm::value_ptr(light.lightAmbi));
    glUniform3fv(u_w_lightPos, 1, glm::value_ptr(light.pos));

    glUniform1i(u_basicTex, 0);

    glBindVertexArray(vaoFloor);
    glDrawArrays(GL_TRIANGLES, 0, pCount);

    glUseProgram(0);
    glBindVertexArray(0);
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}

}
