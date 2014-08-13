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
    //camera = sq::Camera({2.5f, 2.5f, 8.5f}, 0.f, 0.f, 16, 9, 1.17f, 0.1f, 64.f);
    camera.update_projMat();
    camera.update_viewMat();
    camera.update_invProjViewMat();
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
        camera.update_projMat();
        camera.update_viewMat();
        camera.update_invProjViewMat();

        updateCamera = true;
    }
}

void SceneGame::render(sf::RenderTarget& target, float) {
    target.popGLStates();

    static bool first = true;

    static GLuint mainFb, mainFbTexNorm, mainFbTexDiff, mainFbTexSpec, mainFbTexDepth;
    static GLenum mainFbDrawBuffers[] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };

    static GLuint ssaoFb, ssaoFbTex, ssaoFbTexDepth;
    static GLenum ssaoFbDrawBuffers[] = {
        GL_COLOR_ATTACHMENT0
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
    static GLuint texRandnorm;

    static GLuint groundProg = glCreateProgram();
    static GLuint mapModelsProg = glCreateProgram();
    static GLuint ssaoProg = glCreateProgram();
    static GLuint constructProg = glCreateProgram();

    static GLuint groundU_projMat, groundU_viewMat,
                  groundU_texNormArray, groundU_texDiffArray, groundU_texSpecArray;

    static GLuint mapModelsU_projMat, mapModelsU_viewMat, mapModelsU_modelMat,
                  mapModelsU_texNorm, mapModelsU_texDiff, mapModelsU_texSpec,
                  mapModelsU_hasNorm, mapModelsU_hasSpec;

    static GLuint ssaoU_invProjViewMat, ssaoU_viewMat, ssaoU_texNorm, ssaoU_texRandnorm;

    static GLuint constructU_invProjViewMat,
                  constructU_camPos, constructU_skyLightDir,
                  constructU_skyLightDiff, constructU_skyLightAmbi, constructU_skyLightSpec,
                  constructU_texNorm, constructU_texDiff,
                  constructU_texAmbi, constructU_texSpec, constructU_texDepth;

    if (first) {
        glDepthFunc(GL_LEQUAL);
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

        glGenTextures(1, &mainFbTexSpec);
        glBindTexture(GL_TEXTURE_2D, mainFbTexSpec);
        SET_BOTH_NEAREST; SET_BOTH_CLAMP_EDGE;

        glGenTextures(1, &mainFbTexDepth);
        glBindTexture(GL_TEXTURE_2D, mainFbTexDepth);
        SET_BOTH_NEAREST; SET_BOTH_CLAMP_EDGE;
        }

        // SSAO Framebuffer
        {
        glGenFramebuffers(1, &ssaoFb);

        glGenTextures(1, &ssaoFbTex);
        glBindTexture(GL_TEXTURE_2D, ssaoFbTex);
        SET_BOTH_NEAREST; SET_BOTH_CLAMP_EDGE;

        glGenTextures(1, &ssaoFbTexDepth);
        glBindTexture(GL_TEXTURE_2D, ssaoFbTexDepth);
        SET_BOTH_NEAREST; SET_BOTH_CLAMP_EDGE;
        }

        sf::Image img;
        img.loadFromFile("res/misc/randnorms.png");
        glGenTextures(1, &texRandnorm);
        glBindTexture(GL_TEXTURE_2D, texRandnorm);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 64, 64, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
        sq::create_shader("res/shaders/ssao_vs.glsl",
                          "res/shaders/ssao_fs.glsl", ssaoProg);
        sq::create_shader("res/shaders/construct_vs.glsl",
                          "res/shaders/construct_fs.glsl", constructProg);

        groundU_projMat      = glGetUniformLocation(groundProg, "projMat");
        groundU_viewMat      = glGetUniformLocation(groundProg, "viewMat");
        groundU_texNormArray = glGetUniformLocation(groundProg, "texNormArray");
        groundU_texDiffArray = glGetUniformLocation(groundProg, "texDiffArray");
        groundU_texSpecArray = glGetUniformLocation(groundProg, "texSpecArray");

        mapModelsU_projMat      = glGetUniformLocation(mapModelsProg, "projMat");
        mapModelsU_viewMat      = glGetUniformLocation(mapModelsProg, "viewMat");
        mapModelsU_modelMat     = glGetUniformLocation(mapModelsProg, "modelMat");
        mapModelsU_texNorm      = glGetUniformLocation(mapModelsProg, "texNorm");
        mapModelsU_texDiff      = glGetUniformLocation(mapModelsProg, "texDiff");
        mapModelsU_texSpec      = glGetUniformLocation(mapModelsProg, "texSpec");

        ssaoU_invProjViewMat = glGetUniformLocation(ssaoProg, "invProjViewMat");
        ssaoU_viewMat = glGetUniformLocation(ssaoProg, "viewMat");
        ssaoU_texNorm = glGetUniformLocation(ssaoProg, "texNorm");
        ssaoU_texRandnorm = glGetUniformLocation(ssaoProg, "texRandnorm");

        constructU_invProjViewMat = glGetUniformLocation(constructProg, "invProjViewMat");
        constructU_camPos         = glGetUniformLocation(constructProg, "camPos");
        constructU_skyLightDir    = glGetUniformLocation(constructProg, "skyLightDir");
        constructU_skyLightDiff   = glGetUniformLocation(constructProg, "skyLightDiff");
        constructU_skyLightAmbi   = glGetUniformLocation(constructProg, "skyLightAmbi");
        constructU_skyLightSpec   = glGetUniformLocation(constructProg, "skyLightSpec");
        constructU_texNorm        = glGetUniformLocation(constructProg, "texNorm");
        constructU_texDiff        = glGetUniformLocation(constructProg, "texDiff");
        constructU_texAmbi        = glGetUniformLocation(constructProg, "texAmbi");
        constructU_texSpec        = glGetUniformLocation(constructProg, "texSpec");
        constructU_texDepth       = glGetUniformLocation(constructProg, "texDepth");

        glUseProgram(groundProg);
        glUniform1i(groundU_texNormArray, 0);
        glUniform1i(groundU_texDiffArray, 1);
        glUniform1i(groundU_texSpecArray, 2);

        glUseProgram(mapModelsProg);
        glUniform1i(mapModelsU_texNorm, 0);
        glUniform1i(mapModelsU_texDiff, 1);
        glUniform1i(mapModelsU_texSpec, 2);

        glUseProgram(ssaoProg);
        glUniform1i(ssaoU_texNorm, 0);
        glUniform1i(ssaoU_texRandnorm, 1);

        glUseProgram(constructProg);
        glUniform1i(constructU_texNorm, 0);
        glUniform1i(constructU_texDiff, 1);
        glUniform1i(constructU_texAmbi, 2);
        glUniform1i(constructU_texSpec, 3);
        glUniform1i(constructU_texDepth, 4);

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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, mainFbTexNorm, 0);

        glBindTexture(GL_TEXTURE_2D, mainFbTexDiff);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                               GL_TEXTURE_2D, mainFbTexDiff, 0);

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

        // SSAO Framebuffer
        {
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFb);

        glBindTexture(GL_TEXTURE_2D, ssaoFbTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, ssaoFbTex, 0);

        glBindTexture(GL_TEXTURE_2D, ssaoFbTexDepth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height,
                     0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, ssaoFbTexDepth, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "SSAO FBO Error" << std::endl;
        }
        }

        updateFramebuffers = false;
    }

    if (updateSkyLight) {
        glUseProgram(constructProg);
        glUniform3fv(constructU_skyLightDir, 1, glm::value_ptr(skyLight.dir));
        glUniform3fv(constructU_skyLightDiff, 1, glm::value_ptr(skyLight.diff));
        glUniform3fv(constructU_skyLightAmbi, 1, glm::value_ptr(skyLight.ambi));
        glUniform3fv(constructU_skyLightSpec, 1, glm::value_ptr(skyLight.spec));

        updateSkyLight = false;
    }

    if (updateCamera) {
        glUseProgram(groundProg);
        glUniformMatrix4fv(groundU_projMat, 1, GL_FALSE, glm::value_ptr(camera.projMat));
        glUniformMatrix4fv(groundU_viewMat, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

        glUseProgram(mapModelsProg);
        glUniformMatrix4fv(mapModelsU_projMat, 1, GL_FALSE, glm::value_ptr(camera.projMat));
        glUniformMatrix4fv(mapModelsU_viewMat, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

        glUseProgram(constructProg);
        glUniformMatrix4fv(constructU_invProjViewMat, 1, GL_FALSE, glm::value_ptr(camera.invProjViewMat));
        glUniform3fv(constructU_camPos, 1, glm::value_ptr(camera.pos));

        glUseProgram(ssaoProg);
        glUniformMatrix4fv(ssaoU_invProjViewMat, 1, GL_FALSE, glm::value_ptr(camera.invProjViewMat));
        glUniformMatrix4fv(ssaoU_viewMat, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

        updateCamera = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mainFb);
    glDrawBuffers(3, mainFbDrawBuffers);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Ground ///

    glUseProgram(groundProg);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, levelMap.ground.texNormArray);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, levelMap.ground.texDiffArray);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D_ARRAY, levelMap.ground.texSpecArray);

    // Draw
    glBindVertexArray(levelMap.ground.vao);
    glDrawArrays(GL_TRIANGLES, 0, levelMap.ground.pCount);

    /// Map Models ///

    // Model
    glUseProgram(mapModelsProg);
    for (int i = 0; i < levelMap.modelVec.size(); i++) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, levelMap.modelVec[i].texNorm);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, levelMap.modelVec[i].texDiff);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, levelMap.modelVec[i].texSpec);
        for (ModelInstance& m : levelMap.mapModelVec) {
            if (m.index == i) {
                glUniformMatrix4fv(mapModelsU_modelMat, 1, GL_FALSE, glm::value_ptr(m.modelMat));
                glBindVertexArray(levelMap.modelVec[i].vao);
                glDrawArrays(GL_TRIANGLES, 0, levelMap.modelVec[m.index].vCount);
            }
        }
    }


    /// SSAO ///
    glUseProgram(ssaoProg);
    //glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFb);
    glDrawBuffers(1, ssaoFbDrawBuffers);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(screenQuadVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainFbTexNorm);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texRandnorm);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    /// Construct ///
    glUseProgram(constructProg);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(screenQuadVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainFbTexNorm);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mainFbTexDiff);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ssaoFbTex);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mainFbTexSpec);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, mainFbTexDepth);
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
