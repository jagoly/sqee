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

    camera = sq::Camera({2.5f, -1.5f, 8.5f}, 0.375f, 0.f, 16, 9, 1.17f, 0.1f, 100.f);
    //camera = sqe::Camera({0.f, 0.f, 4.f}, 0.f, 0.f, 16, 9, 1.17f, 0.1f, 100.f);
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
    static GLuint modelShadowProg = glCreateProgram();

    static GLuint groundU_projMatrix, groundU_viewMatrix,
                  groundU_w_camPos, groundU_w_lightDir,
                  groundU_lightDiff, groundU_lightAmbi,
                  groundU_texArray, groundU_nMapArray;

    static GLuint mapModelsU_modelMatrix, mapModelsU_projMatrix, mapModelsU_viewMatrix,
                  mapModelsU_w_camPos, mapModelsU_texArray,
                  mapModelsU_w_lightDir, mapModelsU_lightDiff, mapModelsU_lightAmbi;

    static GLuint modelShadowU_modelMatrix, modelShadowU_projMatrix, modelShadowU_viewMatrix,
                  modelShadowU_tex;

    static GLuint playerTex;

    if (first) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LESS);

        levelMap.load_ground();
        levelMap.load_models();

        sq::create_shader("res/shaders/ground_vs.glsl",
                           "res/shaders/ground_fs.glsl", groundProg);
        sq::create_shader("res/shaders/mapmodel_vs.glsl",
                           "res/shaders/mapmodel_fs.glsl", mapModelsProg);
        sq::create_shader("res/shaders/modelshadow_vs.glsl",
                          "res/shaders/modelshadow_fs.glsl", modelShadowProg);

        groundU_projMatrix = glGetUniformLocation(groundProg, "projMatrix");
        groundU_viewMatrix = glGetUniformLocation(groundProg, "viewMatrix");
        groundU_w_camPos = glGetUniformLocation(groundProg, "w_camPos");
        groundU_w_lightDir = glGetUniformLocation(groundProg, "w_lightDir");
        groundU_lightDiff = glGetUniformLocation(groundProg, "lightDiff");
        groundU_lightAmbi = glGetUniformLocation(groundProg, "lightAmbi");
        groundU_texArray = glGetUniformLocation(groundProg, "texArray");
        groundU_nMapArray = glGetUniformLocation(groundProg, "nMapArray");

        mapModelsU_modelMatrix = glGetUniformLocation(mapModelsProg, "modelMatrix");
        mapModelsU_projMatrix = glGetUniformLocation(mapModelsProg, "projMatrix");
        mapModelsU_viewMatrix = glGetUniformLocation(mapModelsProg, "viewMatrix");
        mapModelsU_w_camPos = glGetUniformLocation(mapModelsProg, "w_camPos");
        mapModelsU_w_lightDir = glGetUniformLocation(mapModelsProg, "w_lightDir");
        mapModelsU_lightDiff = glGetUniformLocation(mapModelsProg, "lightDiff");
        mapModelsU_lightAmbi = glGetUniformLocation(mapModelsProg, "lightAmbi");
        mapModelsU_texArray = glGetUniformLocation(mapModelsProg, "texArray");

        modelShadowU_modelMatrix = glGetUniformLocation(modelShadowProg, "modelMatrix");
        modelShadowU_projMatrix = glGetUniformLocation(modelShadowProg, "projMatrix");
        modelShadowU_viewMatrix = glGetUniformLocation(modelShadowProg, "viewMatrix");

        glUseProgram(groundProg);
        glUniform1i(groundU_texArray, 0);
        glUniform1i(groundU_nMapArray, 1);

        glUseProgram(mapModelsProg);
        glUniform1i(mapModelsU_texArray, 0);

        glUseProgram(modelShadowProg);
        glUniform1i(modelShadowU_tex, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
        first = false;
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Ground ///

    // Uniforms
    glUseProgram(groundProg);
    glUniformMatrix4fv(groundU_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
    glUniformMatrix4fv(groundU_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));
    glUniform3fv(groundU_lightDiff, 1, glm::value_ptr(light.lightDiff));
    glUniform3fv(groundU_lightAmbi, 1, glm::value_ptr(light.lightAmbi));
    glUniform3fv(groundU_w_lightDir, 1, glm::value_ptr(light.dir));
    glUniform3fv(groundU_w_camPos, 1, glm::value_ptr(camera.pos));

    // Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, levelMap.ground.texArray);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, levelMap.ground.nMapArray);

    // Draw
    glBindVertexArray(levelMap.ground.vao);
    glDrawArrays(GL_TRIANGLES, 0, levelMap.ground.pCount);

    /// Map Models and Shadows ///

    // Uniforms
    glUseProgram(mapModelsProg);
    glUniformMatrix4fv(mapModelsU_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
    glUniformMatrix4fv(mapModelsU_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));
    glUniform3fv(mapModelsU_lightDiff, 1, glm::value_ptr(light.lightDiff));
    glUniform3fv(mapModelsU_lightAmbi, 1, glm::value_ptr(light.lightAmbi));
    glUniform3fv(mapModelsU_w_lightDir, 1, glm::value_ptr(light.dir));
    glUniform3fv(mapModelsU_w_camPos, 1, glm::value_ptr(camera.pos));

    glUseProgram(modelShadowProg);
    glUniformMatrix4fv(modelShadowU_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
    glUniformMatrix4fv(modelShadowU_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

    glActiveTexture(GL_TEXTURE0); // used throughout models
    for (int i = 0; i < levelMap.modelVec.size(); i++) {
        glUseProgram(mapModelsProg);
        glBindTexture(GL_TEXTURE_2D_ARRAY, levelMap.modelVec[i].texArray);
        glUseProgram(modelShadowProg);
        glBindTexture(GL_TEXTURE_2D, levelMap.modelVec[i].shadTex);
        for (ModelInstance& m : levelMap.mapModelVec) {
            if (m.index == i) {
                glUseProgram(mapModelsProg);
                glUniformMatrix4fv(mapModelsU_modelMatrix, 1, GL_FALSE, glm::value_ptr(m.modelMatrix));
                glBindVertexArray(levelMap.modelVec[i].vao);
                glDrawArrays(GL_TRIANGLES, 0, levelMap.modelVec[m.index].vCount);

                glUseProgram(modelShadowProg);
                glBindVertexArray(levelMap.modelVec[i].shadVao);
                glUniformMatrix4fv(modelShadowU_modelMatrix, 1, GL_FALSE, glm::value_ptr(m.modelMatrix));
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
    }

    /// Player ///


    glUseProgram(0);
    glBindVertexArray(0);

    target.pushGLStates();
}

bool HandlerGame::handle(sf::Event&) {
    return false;
}

}
