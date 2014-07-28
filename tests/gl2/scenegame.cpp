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

    levelMap.load_map("res/maps/test");

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

    camera.pos.x += EW * 0.02f;
    camera.pos.y += NS * 0.02f;

    camera.update_projMatrix();
    camera.update_viewMatrix();
}

void SceneGame::render(sf::RenderTarget& target, float) {
    target.popGLStates();

    static bool first = true;

    static GLuint groundProg = glCreateProgram();
    static GLuint mapModelsProg = glCreateProgram();

    static GLuint groundU_projMatrix, groundU_viewMatrix,
                  groundU_w_lightPos, groundU_lightDiff, groundU_lightAmbi,
                  groundU_texArray, groundU_nMapArray;

    static GLuint mapModelsU_modelMatrix, mapModelsU_projMatrix, mapModelsU_viewMatrix,
                  mapModelsU_w_lightPos, mapModelsU_lightDiff, mapModelsU_lightAmbi;
                  //groundU_tex, groundU_nMap;

    if (first) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        levelMap.load_ground();
        levelMap.load_models();

        sqe::create_shader("res/shaders/ground_vs.glsl",
                           "res/shaders/ground_fs.glsl", groundProg);
        sqe::create_shader("res/shaders/mapmodel_vs.glsl",
                           "res/shaders/mapmodel_fs.glsl", mapModelsProg);

        groundU_projMatrix = glGetUniformLocation(groundProg, "projMatrix");
        groundU_viewMatrix = glGetUniformLocation(groundProg, "viewMatrix");
        groundU_w_lightPos = glGetUniformLocation(groundProg, "w_lightPos");
        groundU_lightDiff = glGetUniformLocation(groundProg, "lightDiff");
        groundU_lightAmbi = glGetUniformLocation(groundProg, "lightAmbi");
        groundU_texArray = glGetUniformLocation(groundProg, "texArray");
        groundU_nMapArray = glGetUniformLocation(groundProg, "nMapArray");

        mapModelsU_modelMatrix = glGetUniformLocation(mapModelsProg, "modelMatrix");
        mapModelsU_projMatrix = glGetUniformLocation(mapModelsProg, "projMatrix");
        mapModelsU_viewMatrix = glGetUniformLocation(mapModelsProg, "viewMatrix");
        mapModelsU_w_lightPos = glGetUniformLocation(mapModelsProg, "w_lightPos");
        mapModelsU_lightDiff = glGetUniformLocation(mapModelsProg, "lightDiff");
        mapModelsU_lightAmbi = glGetUniformLocation(mapModelsProg, "lightAmbi");
        //mapModelsU_tex = glGetUniformLocation(mapModelsProg, "tex");
        //mapModelsU_nMap = glGetUniformLocation(mapModelsProg, "nMap");

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        first = false;
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Ground ///

    glUseProgram(groundProg);

    glUniformMatrix4fv(groundU_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
    glUniformMatrix4fv(groundU_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

    glUniform3fv(groundU_lightDiff, 1, glm::value_ptr(light.lightDiff));
    glUniform3fv(groundU_lightAmbi, 1, glm::value_ptr(light.lightAmbi));
    glUniform3fv(groundU_w_lightPos, 1, glm::value_ptr(light.pos));

    glUniform1i(groundU_texArray, 0);
    glUniform1i(groundU_nMapArray, 1);

    glBindVertexArray(levelMap.ground.vao);
    glDrawArrays(GL_TRIANGLES, 0, levelMap.ground.pCount);

    /// Map Models ///

    glUseProgram(mapModelsProg);

    glUniformMatrix4fv(mapModelsU_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
    glUniformMatrix4fv(mapModelsU_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

    glUniform3fv(mapModelsU_lightDiff, 1, glm::value_ptr(light.lightDiff));
    glUniform3fv(mapModelsU_lightAmbi, 1, glm::value_ptr(light.lightAmbi));
    glUniform3fv(mapModelsU_w_lightPos, 1, glm::value_ptr(light.pos));

    for (int i = 0; i < levelMap.modelVec.size(); i++) {
        // bind texture here
        for (ModelInstance& m : levelMap.mapModelVec) {
            if (m.index == i) {
                glUniformMatrix4fv(mapModelsU_modelMatrix, 1, GL_FALSE, glm::value_ptr(m.modelMatrix));
                glBindVertexArray(levelMap.modelVec[m.index].vao);
                glDrawArrays(GL_TRIANGLES, 0, levelMap.modelVec[m.index].pCount);
            }
        }
    }

    glUseProgram(0);
    glBindVertexArray(0);

    target.pushGLStates();
}

bool HandlerGame::handle(sf::Event&) {
    return false;
}

}
