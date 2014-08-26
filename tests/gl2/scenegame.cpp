#include "scenegame.hpp"

#include <iostream>
#include <random>

#include <libsqee/app/application.hpp>
#include <libsqee/gl/shaders.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/gl/framebuffers.hpp>

#include "helpers.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;

    level.load_map("new");

    camera = sq::Camera({2.5f, -1.5f, 8.5f}, 0.375f, 0.f, 16, 9, 1.17f, 0.1f, 64.f);
    //camera = sq::Camera({2.5f, 2.5f, 8.5f}, 0.f, 0.f, 16, 9, 1.17f, 0.1f, 64.f);
    camera.update_projMat();
    camera.update_viewMat();
    camera.update_projViewMat();
}

void SceneGame::resize(glm::uvec2 _size) {
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
        camera.update_projViewMat();

        updateCamera = true;
    }
}

void SceneGame::render(float) {
    static bool first = true;

    static std::minstd_rand gen(std::time(NULL)); gen();

    static sq::Framebuffer mapFb;
    static GLenum mapFbDrawBuffers[] = {
        gl::COLOR_ATTACHMENT0, gl::COLOR_ATTACHMENT1,
        gl::COLOR_ATTACHMENT2, gl::COLOR_ATTACHMENT3
    };

    static sq::Framebuffer lightingFb;
    static GLenum lightingFbDrawBuffers[] = {
        gl::COLOR_ATTACHMENT0
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

    static sq::Shader terrainProg;
    static sq::Shader mapModelProg;
    static sq::Shader lightingProg;
    static sq::Shader fxaaProg;

    if (first) {
        gl::DepthFunc(gl::LEQUAL);
        gl::ClearColor(0.4f, 0.2f, 0.6f, 1.f);

        level.load_ground();
        level.load_models();

        // Main Framebuffer
        mapFb.create(4, mapFbDrawBuffers, true);
        lightingFb.create(1, lightingFbDrawBuffers, false);

        GLuint vboPoints, vboTexcoords;

        gl::GenBuffers(1, &vboPoints);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboPoints);
        gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(GLfloat), screenQuadPoints, gl::STATIC_DRAW);

        gl::GenBuffers(1, &vboTexcoords);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboTexcoords);
        gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(GLfloat), screenQuadTexcoords, gl::STATIC_DRAW);

        gl::GenVertexArrays(1, &screenQuadVao);
        gl::BindVertexArray(screenQuadVao);
        gl::EnableVertexAttribArray(0);
        gl::EnableVertexAttribArray(1);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboPoints);
        gl::VertexAttribPointer(0, 2, gl::FLOAT, false, 0, NULL);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboTexcoords);
        gl::VertexAttribPointer(1, 2, gl::FLOAT, false, 0, NULL);

        terrainProg.load_from_file("res/shaders/terrain_vs.glsl", gl::VERTEX_SHADER);
        terrainProg.load_from_file("res/shaders/terrain_fs.glsl", gl::FRAGMENT_SHADER);
        terrainProg.build();
        mapModelProg.load_from_file("res/shaders/mapmodel_vs.glsl", gl::VERTEX_SHADER);
        mapModelProg.load_from_file("res/shaders/mapmodel_fs.glsl", gl::FRAGMENT_SHADER);
        mapModelProg.build();
        lightingProg.load_from_file("res/shaders/construct_vs.glsl", gl::VERTEX_SHADER);
        lightingProg.load_from_file("res/shaders/construct_fs.glsl", gl::FRAGMENT_SHADER);
        lightingProg.build();
        fxaaProg.load_from_file("res/shaders/fxaa_vs.glsl", gl::VERTEX_SHADER);
        fxaaProg.load_from_file("res/shaders/fxaa_fs.glsl", gl::FRAGMENT_SHADER);
        fxaaProg.build();

        terrainProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        terrainProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        terrainProg.add_uniform("mapSize", sq::U_TYPE::u_2i);
        terrainProg.add_uniform("texAmbi", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("texNormArray", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("texDiffArray", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("texSpecArray", sq::U_TYPE::u_1i);

        mapModelProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        mapModelProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        mapModelProg.add_uniform("modelMat", sq::U_TYPE::u_4m);
        mapModelProg.add_uniform("texNorm", sq::U_TYPE::u_1i);
        mapModelProg.add_uniform("texDiff", sq::U_TYPE::u_1i);
        mapModelProg.add_uniform("texAmbi", sq::U_TYPE::u_1i);
        mapModelProg.add_uniform("texSpec", sq::U_TYPE::u_1i);

        lightingProg.add_uniform("invProjMat", sq::U_TYPE::u_4m);
        lightingProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        lightingProg.add_uniform("camPos", sq::U_TYPE::u_3f);
        lightingProg.add_uniform("skyLightDir", sq::U_TYPE::u_3f);
        lightingProg.add_uniform("skyLightDiff", sq::U_TYPE::u_3f);
        lightingProg.add_uniform("skyLightAmbi", sq::U_TYPE::u_3f);
        lightingProg.add_uniform("skyLightSpec", sq::U_TYPE::u_3f);
        lightingProg.add_uniform("texNorm", sq::U_TYPE::u_1i);
        lightingProg.add_uniform("texDiff", sq::U_TYPE::u_1i);
        lightingProg.add_uniform("texAmbi", sq::U_TYPE::u_1i);
        lightingProg.add_uniform("texSpec", sq::U_TYPE::u_1i);
        lightingProg.add_uniform("texDepth", sq::U_TYPE::u_1i);

        fxaaProg.add_uniform("tex", sq::U_TYPE::u_1i);
        fxaaProg.add_uniform("aaLevel", sq::U_TYPE::u_1i);

        terrainProg.use();
        GLint mapSize[2] = {15, 7};
        terrainProg.set_uniform_iv("mapSize", mapSize);
        terrainProg.set_uniform_i("texNormArray", 0);
        terrainProg.set_uniform_i("texDiffArray", 1);
        terrainProg.set_uniform_i("texAmbi", 2);
        terrainProg.set_uniform_i("texSpecArray", 3);

        mapModelProg.use();
        mapModelProg.set_uniform_i("texNorm", 0);
        mapModelProg.set_uniform_i("texDiff", 1);
        mapModelProg.set_uniform_i("texAmbi", 2);
        mapModelProg.set_uniform_i("texSpec", 3);

        lightingProg.use();
        lightingProg.set_uniform_i("texNorm", 0);
        lightingProg.set_uniform_i("texDiff", 1);
        lightingProg.set_uniform_i("texAmbi", 2);
        lightingProg.set_uniform_i("texSpec", 3);
        lightingProg.set_uniform_i("texDepth", 4);

        fxaaProg.use();
        fxaaProg.set_uniform_i("tex", 0);
        fxaaProg.set_uniform_i("aaLevel", 1);

        gl::BindBuffer(gl::ARRAY_BUFFER, 0);
        gl::UseProgram(0);

        first = false;
    }

    if (updateFramebuffers) {
        glm::uvec2 size = app->get_size();
        // Main Framebuffer
        mapFb.resize(size);
        lightingFb.resize(size);
        lightingFb.colourTextures[0]->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);

        updateFramebuffers = false;
    }

    if (updateSkyLight) {
        lightingProg.use();
        lightingProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        lightingProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        lightingProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        lightingProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));

        updateSkyLight = false;
    }

    if (updateCamera) {
        terrainProg.use();
        terrainProg.set_uniform_mv("projMat", false, glm::value_ptr(camera.projMat));
        terrainProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));

        mapModelProg.use();
        mapModelProg.set_uniform_mv("projMat", false, glm::value_ptr(camera.projMat));
        mapModelProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));

        lightingProg.use();
        lightingProg.set_uniform_mv("invProjMat", false, glm::value_ptr(glm::inverse(camera.projMat)));
        lightingProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));
        lightingProg.set_uniform_fv("camPos", glm::value_ptr(camera.pos));

        updateCamera = false;
    }

    mapFb.use();
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);


    /// Ground ///
    gl::Enable(gl::DEPTH_TEST);

    terrainProg.use();

    // Textures
    level.terrain.texNormArray.bind(gl::TEXTURE0);
    level.terrain.texDiffArray.bind(gl::TEXTURE1);
    level.terrain.texAmbi.bind(gl::TEXTURE2);
    level.terrain.texSpecArray.bind(gl::TEXTURE3);

    // Draw
    gl::BindVertexArray(level.terrain.vao);
    gl::DrawArrays(gl::TRIANGLES, 0, level.terrain.vCount);


    /// Map Models ///

    mapModelProg.use();
    for (int i = 0; i < level.modelVec.size(); i++) {
        level.modelVec[i].texNorm.bind(gl::TEXTURE0);
        level.modelVec[i].texDiff.bind(gl::TEXTURE1);
        level.modelVec[i].texSpec.bind(gl::TEXTURE3);
        gl::BindVertexArray(level.modelVec[i].vao);
        for (ModelInstance& m : level.modelInstVec) {
            if (m.index == i) {
                mapModelProg.set_uniform_mv("modelMat", false, glm::value_ptr(m.modelMat));
                m.texAmbi.bind(gl::TEXTURE2);
                gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, level.modelVec[m.index].ibo);
                gl::DrawElements(gl::TRIANGLES, level.modelVec[m.index].iCount, gl::UNSIGNED_SHORT, 0);
            }
        }
    }


    /// Lightnig PP ///
    gl::Disable(gl::BLEND);
    gl::Disable(gl::DEPTH_TEST);

    lightingFb.use();
    gl::Clear(gl::COLOR_BUFFER_BIT);
    lightingProg.use();

    mapFb.colourTextures[0]->bind(gl::TEXTURE0);
    mapFb.colourTextures[1]->bind(gl::TEXTURE1);
    mapFb.colourTextures[2]->bind(gl::TEXTURE2);
    mapFb.colourTextures[3]->bind(gl::TEXTURE3);
    mapFb.depthTexture.bind(gl::TEXTURE4);
    gl::BindVertexArray(screenQuadVao);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);

    /// FXAA and screen output ///
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    gl::Clear(gl::COLOR_BUFFER_BIT);
    fxaaProg.use();

    lightingFb.colourTextures[0]->bind(gl::TEXTURE0);
    gl::BindVertexArray(screenQuadVao);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
}

bool HandlerGame::handle(sf::Event&) {
    return false;
}

}
