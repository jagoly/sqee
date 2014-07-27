#include "scenegame.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libsqee/application.hpp>

#include "helpers.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;

    ground.load_map("res/maps/test.json");

    camera = sqe::Camera({0.f, -3.f, 8.f}, 0.5f, 0.f, 16, 9, 1.17f, 0.1f, 100.f);
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
    target.popGLStates();

    static bool first = true;

    static GLuint prog = glCreateProgram();
    static GLuint u_projMatrix, u_viewMatrix,
                  u_w_lightPos, u_lightDiff, u_lightAmbi,
                  u_texArray, u_nMapArray;

    static GLuint groundTexArray = 0;
    static GLuint groundNMapArray = 0;
    static int pCount;
    static GLuint vaoGround = 0;

    if (first) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        ground.get_models(vaoGround, groundTexArray, groundNMapArray, pCount);

        sqe::create_shader("res/shaders/main_vs.glsl", "res/shaders/main_fs.glsl", prog);

        u_projMatrix = glGetUniformLocation(prog, "projMatrix");
        u_viewMatrix = glGetUniformLocation(prog, "viewMatrix");
        u_w_lightPos = glGetUniformLocation(prog, "w_lightPos");
        u_lightDiff = glGetUniformLocation(prog, "lightDiff");
        u_lightAmbi = glGetUniformLocation(prog, "lightAmbi");
        u_texArray = glGetUniformLocation(prog, "texArray");
        u_nMapArray = glGetUniformLocation(prog, "nMapArray");

        first = false;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(prog);

    glUniformMatrix4fv(u_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
    glUniformMatrix4fv(u_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

    glUniform3fv(u_lightDiff, 1, glm::value_ptr(light.lightDiff));
    glUniform3fv(u_lightAmbi, 1, glm::value_ptr(light.lightAmbi));
    glUniform3fv(u_w_lightPos, 1, glm::value_ptr(light.pos));

    glUniform1i(u_texArray, 0);
    glUniform1i(u_nMapArray, 1);

    glBindVertexArray(vaoGround);
    glDrawArrays(GL_TRIANGLES, 0, pCount);

    glUseProgram(0);
    glBindVertexArray(0);

    target.pushGLStates();
}

bool HandlerGame::handle(sf::Event&) {
    return false;
}

}
