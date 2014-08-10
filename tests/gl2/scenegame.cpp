#include "scenegame.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libsqee/application.hpp>

#include "helpers.hpp"

#define SET_BOTH_NEAREST \
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);\
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)

#define SET_BOTH_CLAMP_EDGE \
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);\
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;

    levelMap.load_map("res/maps/test");

    camera = sq::Camera({2.5f, -1.5f, 8.5f}, 0.375f, 0.f, 16, 9, 1.17f, 0.1f, 64.f);
    //camera = sq::Camera({2.5f, -1.5f, 2.5f}, 0.75f, 0.f, 16, 9, 1.17f, 0.1f, 64.f);
    camera.update_projMatrix();
    camera.update_viewMatrix();
}

void SceneGame::resize(unsigned int width, unsigned int height) {
    updateFramebuffers = true;
}

void SceneGame::update() {
    int NS = 0;
    int EW = 0;
    bool update = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { EW += 1; update = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  { EW -= 1; update = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    { NS += 1; update = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  { NS -= 1; update = true; }

    if (update) {
        camera.pos.x += EW * 0.02f;
        camera.pos.y += NS * 0.02f;
        camera.update_projMatrix();
        camera.update_viewMatrix();

        updateCamera = true;
    }
}

void SceneGame::render(sf::RenderTarget& target, float) {
    target.popGLStates();

    static bool first = true;

    static GLuint mainFb, mainFbTexNorm, mainFbTexDiff, mainFbTexAmbi, mainFbTexSpec, mainFbTexDepth;
    static GLenum mainFbDrawBuffers[] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
    };

    static GLfloat screenQuadPoints[] = {
        -1.0, -1.0,  1.0,  -1.0,  1.0, 1.0,
        1.0, 1.0,    -1.0, 1.0,   -1.0, -1.0
    };
    static GLfloat screenQuadTexcoords[] = {
        0.0, 0.0,  1.0, 0.0,  1.0, 1.0,
        1.0, 1.0,  0.0, 1.0,  0.0, 0.0
    };
    static GLuint screenQuadVao;

    static GLuint groundProg = glCreateProgram();
    static GLuint mapModelsProg = glCreateProgram();
    static GLuint constructProg = glCreateProgram();

    static GLuint groundU_projMatrix, groundU_viewMatrix,
                  groundU_camPos, groundU_skyLightDir,
                  groundU_skyLightDiff, groundU_skyLightAmbi, groundU_skyLightSpec,
                  groundU_texDiffArray, groundU_texNormArray;

    static GLuint mapModelsU_modelMatrix, mapModelsU_projMatrix, mapModelsU_viewMatrix,
                  mapModelsU_camPos, mapModelsU_skyLightDir,
                  mapModelsU_skyLightDiff, mapModelsU_skyLightAmbi, mapModelsU_skyLightSpec,
                  mapModelsU_texNorm, mapModelsU_texShadow,
                  mapModelsU_texDiff, mapModelsU_texAmbi, mapModelsU_texSpec,
                  mapModelsU_hasNorm, mapModelsU_hasShadow,
                  mapModelsU_hasAmbi, mapModelsU_hasSpec;

    static GLuint constructU_texNorm, constructU_texDiff,
                  constructU_texAmbi, constructU_texSpec;

    if (first) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        levelMap.load_ground();
        levelMap.load_models();

        // Main Framebuffer
        {
        glGenFramebuffers(1, &mainFb);

        glGenTextures(1, &mainFbTexNorm);
        glBindTexture(GL_TEXTURE_2D, mainFbTexNorm);
        SET_BOTH_NEAREST; SET_BOTH_CLAMP_EDGE;

        glGenTextures(1, &mainFbTexDiff);
        glBindTexture(GL_TEXTURE_2D, mainFbTexDiff);
        SET_BOTH_NEAREST; SET_BOTH_CLAMP_EDGE;

        glGenTextures(1, &mainFbTexAmbi);
        glBindTexture(GL_TEXTURE_2D, mainFbTexAmbi);
        SET_BOTH_NEAREST; SET_BOTH_CLAMP_EDGE;

        glGenTextures(1, &mainFbTexSpec);
        glBindTexture(GL_TEXTURE_2D, mainFbTexSpec);
        SET_BOTH_NEAREST; SET_BOTH_CLAMP_EDGE;

        glGenTextures(1, &mainFbTexDepth);
        glBindTexture(GL_TEXTURE_2D, mainFbTexDepth);
        SET_BOTH_NEAREST; SET_BOTH_CLAMP_EDGE;
        }


        GLuint vboPoints, vboTexcoords;

        glGenBuffers(1, &vboPoints);
        glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadPoints), screenQuadPoints, GL_STATIC_DRAW);

        glGenBuffers(1, &vboTexcoords);
        glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadTexcoords), screenQuadTexcoords, GL_STATIC_DRAW);

        glGenVertexArrays(1, &screenQuadVao);
        glBindVertexArray(screenQuadVao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

        sq::create_shader("res/shaders/ground_vs.glsl",
                          "res/shaders/ground_fs.glsl", groundProg);
        sq::create_shader("res/shaders/mapmodel_vs.glsl",
                          "res/shaders/mapmodel_fs.glsl", mapModelsProg);
        sq::create_shader("res/shaders/construct_vs.glsl",
                          "res/shaders/construct_fs.glsl", constructProg);

        groundU_projMatrix      = glGetUniformLocation(groundProg, "projMatrix");
        groundU_viewMatrix      = glGetUniformLocation(groundProg, "viewMatrix");
        groundU_camPos          = glGetUniformLocation(groundProg, "camPos");
        groundU_skyLightDir     = glGetUniformLocation(groundProg, "skyLightDir");
        groundU_skyLightDiff    = glGetUniformLocation(groundProg, "skyLightDiff");
        groundU_skyLightAmbi    = glGetUniformLocation(groundProg, "skyLightAmbi");
        groundU_skyLightSpec    = glGetUniformLocation(groundProg, "skyLightSpec");
        groundU_texNormArray    = glGetUniformLocation(groundProg, "texNormArray");
        groundU_texDiffArray    = glGetUniformLocation(groundProg, "texDiffArray");

        mapModelsU_modelMatrix  = glGetUniformLocation(mapModelsProg, "modelMatrix");
        mapModelsU_projMatrix   = glGetUniformLocation(mapModelsProg, "projMatrix");
        mapModelsU_viewMatrix   = glGetUniformLocation(mapModelsProg, "viewMatrix");
        mapModelsU_camPos       = glGetUniformLocation(mapModelsProg, "camPos");
        mapModelsU_skyLightDir  = glGetUniformLocation(mapModelsProg, "skyLightDir");
        mapModelsU_skyLightDiff = glGetUniformLocation(mapModelsProg, "skyLightDiff");
        mapModelsU_skyLightAmbi = glGetUniformLocation(mapModelsProg, "skyLightAmbi");
        mapModelsU_skyLightSpec = glGetUniformLocation(mapModelsProg, "skyLightSpec");
        mapModelsU_texNorm      = glGetUniformLocation(mapModelsProg, "texNorm");
        mapModelsU_texShadow    = glGetUniformLocation(mapModelsProg, "texShadow");
        mapModelsU_texDiff      = glGetUniformLocation(mapModelsProg, "texDiff");
        mapModelsU_texAmbi      = glGetUniformLocation(mapModelsProg, "texAmbi");
        mapModelsU_texSpec      = glGetUniformLocation(mapModelsProg, "texSpec");
        mapModelsU_hasNorm      = glGetUniformLocation(mapModelsProg, "hasNorm");
        mapModelsU_hasShadow    = glGetUniformLocation(mapModelsProg, "hasShadow");
        mapModelsU_hasAmbi      = glGetUniformLocation(mapModelsProg, "hasAmbi");
        mapModelsU_hasSpec      = glGetUniformLocation(mapModelsProg, "hasSpec");

        constructU_texNorm      = glGetUniformLocation(constructProg, "texNorm");
        constructU_texDiff      = glGetUniformLocation(constructProg, "texDiff");
        constructU_texAmbi      = glGetUniformLocation(constructProg, "texAmbi");
        constructU_texSpec      = glGetUniformLocation(constructProg, "texSpec");

        glUseProgram(groundProg);
        glUniform1i(groundU_texNormArray, 0);
        glUniform1i(groundU_texDiffArray, 1);

        glUseProgram(mapModelsProg);
        glUniform1i(mapModelsU_texNorm, 0);
        glUniform1i(mapModelsU_texShadow, 1);
        glUniform1i(mapModelsU_texDiff, 2);
        glUniform1i(mapModelsU_texAmbi, 3);
        glUniform1i(mapModelsU_texSpec, 4);

        glUseProgram(constructProg);
        glUniform1i(constructU_texNorm, 0);
        glUniform1i(constructU_texDiff, 1);
        glUniform1i(constructU_texAmbi, 2);
        glUniform1i(constructU_texSpec, 3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);

        first = false;
    }

    if (updateFramebuffers) {
        int width = target.getSize().x;
        int height = target.getSize().y;
        // Main Framebuffer
        {
        glBindFramebuffer(GL_FRAMEBUFFER, mainFb);

        glBindTexture(GL_TEXTURE_2D, mainFbTexNorm);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, mainFbTexNorm, 0);

        glBindTexture(GL_TEXTURE_2D, mainFbTexDiff);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                               GL_TEXTURE_2D, mainFbTexDiff, 0);

        glBindTexture(GL_TEXTURE_2D, mainFbTexAmbi);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                               GL_TEXTURE_2D, mainFbTexAmbi, 0);

        glBindTexture(GL_TEXTURE_2D, mainFbTexSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
                               GL_TEXTURE_2D, mainFbTexSpec, 0);

        glBindTexture(GL_TEXTURE_2D, mainFbTexDepth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height,
                     0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, mainFbTexDepth, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Primary FBO Error" << std::endl;
        }
        }

        updateFramebuffers = false;
    }

    if (updateSkyLight) {
        glUseProgram(groundProg);
        glUniform3fv(groundU_skyLightDir, 1, glm::value_ptr(skyLight.dir));
        glUniform3fv(groundU_skyLightDiff, 1, glm::value_ptr(skyLight.diff));
        glUniform3fv(groundU_skyLightAmbi, 1, glm::value_ptr(skyLight.ambi));
        glUniform3fv(groundU_skyLightSpec, 1, glm::value_ptr(skyLight.spec));

        glUniform3fv(mapModelsU_skyLightDir, 1, glm::value_ptr(skyLight.dir));
        glUniform3fv(mapModelsU_skyLightDiff, 1, glm::value_ptr(skyLight.diff));
        glUniform3fv(mapModelsU_skyLightAmbi, 1, glm::value_ptr(skyLight.ambi));
        glUniform3fv(mapModelsU_skyLightAmbi, 1, glm::value_ptr(skyLight.spec));

        updateSkyLight = false;
    }

    if (updateCamera) {
        glUseProgram(groundProg);
        glUniformMatrix4fv(groundU_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
        glUniformMatrix4fv(groundU_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));
        glUniform3fv(groundU_camPos, 1, glm::value_ptr(camera.pos));

        glUseProgram(mapModelsProg);
        glUniformMatrix4fv(mapModelsU_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
        glUniformMatrix4fv(mapModelsU_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));
        glUniform3fv(mapModelsU_camPos, 1, glm::value_ptr(camera.pos));

        updateCamera = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mainFb);
    glDrawBuffers(4, mainFbDrawBuffers);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Ground ///

    // Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, levelMap.ground.texNormArray);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, levelMap.ground.texDiffArray);

    // Draw
    glBindVertexArray(levelMap.ground.vao);
    glDrawArrays(GL_TRIANGLES, 0, levelMap.ground.pCount);

    /// Map Models ///



    // Model
//    glUseProgram(mapModelsProg);
//    glActiveTexture(GL_TEXTURE0);
//    for (int i = 0; i < levelMap.modelVec.size(); i++) {
//        glBindTexture(GL_TEXTURE_2D_ARRAY, levelMap.modelVec[i].texArray);
//        for (ModelInstance& m : levelMap.mapModelVec) {
//            if (m.index == i) {
//                glUniformMatrix4fv(mapModelsU_modelMatrix, 1, GL_FALSE, glm::value_ptr(m.modelMatrix));
//                glBindVertexArray(levelMap.modelVec[i].vao);
//                glDrawArrays(GL_TRIANGLES, 0, levelMap.modelVec[m.index].vCount);
//            }
//        }
//    }

//    /// CREATE MODEL SHADOWS ///
//    glUseProgram(modshadProg);
//    glUniformMatrix4fv(modshadU_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
//    glUniformMatrix4fv(modshadU_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

//    glBindFramebuffer(GL_FRAMEBUFFER, modshadFb);
//    glDrawBuffers(1, modshadDrawBuffers);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glBindFramebuffer(GL_READ_FRAMEBUFFER, msFb);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, modshadFb);
//    glBlitFramebuffer(0, 0, target.getSize().x, target.getSize().y,
//                      0, 0, target.getSize().x, target.getSize().y,
//                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

//    glActiveTexture(GL_TEXTURE0);
//    for (int i = 0; i < levelMap.modelVec.size(); i++) {
//        if (levelMap.modelVec[i].hasShadow) {
//            glBindTexture(GL_TEXTURE_2D, levelMap.modelVec[i].texShadow);
//            for (ModelInstance& m : levelMap.mapModelVec) {
//                if (m.index == i && m.hasShadow) {
//                    glUniformMatrix4fv(modshadU_modelMatrix, 1, GL_FALSE, glm::value_ptr(m.modelMatrix));
//                    glBindVertexArray(levelMap.modelVec[i].vaoShadow);
//                    glDrawArrays(GL_TRIANGLES, 0, 6);
//                }
//            }
//        }
//    }

    /// Construct ///
    glUseProgram(constructProg);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(screenQuadVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainFbTexNorm);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mainFbTexDiff);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    // Tidy For SFML
    glUseProgram(0);
    glBindVertexArray(0);
    target.pushGLStates();
}

bool HandlerGame::handle(sf::Event&) {
    return false;
}

}
