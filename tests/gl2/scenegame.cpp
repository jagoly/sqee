#include "scenegame.hpp"

#include <iostream>
#include <random>
#include <array>

#include <libsqee/app/application.hpp>
#include <libsqee/gl/shaders.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/gl/framebuffers.hpp>

#include "helpers.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 64;
    dt = 1.d/64;

    level.load_map("new");

    camera.init({2.5f, -1.5f, 8.5f}, 0.375f, 0, 0, 0, 1.17f, 0.1f, 64.f);
    //camera = sq::Camera({2.5f, 2.5f, 8.5f}, 0.f, 0.f, 16, 9, 1.17f, 0.1f, 64.f);
    camera.update_projMat();
    camera.update_viewMat();
    camera.update_projViewMat();

    player.camera = &camera;
    player.level = &level;
    resize(app->get_size());
}

void SceneGame::resize(glm::uvec2 _size) {
    camera.width = _size.x;
    camera.height = _size.y;
    updateFramebuffers = true;
}

void SceneGame::update() {
    static std::array<ushort, 4> keys = {0, 1, 2, 3};
    static std::array<bool, 4> keyStates = {false, false, false, false};

    std::array<ushort, 4> keysPrev = keys;
    std::array<bool, 4> keyStatesPrev = keyStates;

    keyStates[0] = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    keyStates[1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    keyStates[2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    keyStates[3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);

    ushort keyPrs = 4;
    ushort keyRls = 4;
    for (int k = 0; k < 4; k++) {
        if (keyStates[k] && !keyStatesPrev[k]) keyPrs = k;
        if (!keyStates[k] && keyStatesPrev[k]) keyRls = k;
    }

    if (keyPrs != 4) {
        keys[0] = keyPrs;
        keys[1] = keysPrev[0];
        if (keysPrev[2] == keyPrs || keysPrev[3] == keyPrs)
            keys[2] = keysPrev[1];
        if (keysPrev[3] == keyPrs)
            keys[3] = keysPrev[2];
    }

    if (keyRls != 4) {
        keys[3] = keyRls;
        keys[2] = keysPrev[3];
        if (keysPrev[1] == keyRls || keysPrev[0] == keyRls)
            keys[1] = keysPrev[2];
        if (keysPrev[0] == keyRls)
            keys[0] = keysPrev[1];
    }

    player.moveNext = keyStates[0] || keyStates[1] || keyStates[2] || keyStates[3];
    player.update_logic(keys);
}

void SceneGame::render(float) {
    static bool first = true;
    static int shadLevel, aaLevel;

    static std::minstd_rand gen(std::time(NULL)); gen();

    static sq::Framebuffer shadowFb;
    static GLenum noDrawBuffers[] = {gl::NONE};

    static sq::Framebuffer mapFb;
    static GLenum mapFbDrawBuffers[] = {
        gl::COLOR_ATTACHMENT0
    };

    static sq::ScreenQuad screenQuad;

    static sq::Shader terrainProg;
    static sq::Shader modelOpProg;
    static sq::Shader shadowProg;
    static sq::Shader lumaProg;
    static sq::Shader fxaaLProg;
    static sq::Shader fxaaHProg;
    static sq::Shader quadProg;

    if (first) {
        gl::DepthFunc(gl::LEQUAL);
        gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
        gl::ClearColor(0.1f, 0.05f, 0.2f, 1.f);


        level.load_ground();
        level.load_models();
        level.load_physics();

        mapFb.create(1, mapFbDrawBuffers, true);

        // Shaders
        terrainProg.load_from_file("res/shaders/terrain_vs.glsl", gl::VERTEX_SHADER);
        terrainProg.load_from_file("res/shaders/terrain_fs.glsl", gl::FRAGMENT_SHADER);
        terrainProg.build();
        modelOpProg.load_from_file("res/shaders/modelop_vs.glsl", gl::VERTEX_SHADER);
        modelOpProg.load_from_file("res/shaders/modelop_fs.glsl", gl::FRAGMENT_SHADER);
        modelOpProg.build();
        shadowProg.load_from_file("res/shaders/shadows_vs.glsl", gl::VERTEX_SHADER);
        shadowProg.load_from_file("res/shaders/shadows_fs.glsl", gl::FRAGMENT_SHADER);
        shadowProg.build();
        lumaProg.load_from_file("res/shaders/quad_vs.glsl", gl::VERTEX_SHADER);
        lumaProg.load_from_file("res/shaders/luma_fs.glsl", gl::FRAGMENT_SHADER);
        lumaProg.build();
        fxaaLProg.load_from_file("res/shaders/quad_vs.glsl", gl::VERTEX_SHADER);
        fxaaLProg.load_from_file("res/shaders/fxaal_fs.glsl", gl::FRAGMENT_SHADER);
        fxaaLProg.build();
        fxaaHProg.load_from_file("res/shaders/quad_vs.glsl", gl::VERTEX_SHADER);
        fxaaHProg.load_from_file("res/shaders/fxaah_fs.glsl", gl::FRAGMENT_SHADER);
        fxaaHProg.build();
        quadProg.load_from_file("res/shaders/quad_vs.glsl", gl::VERTEX_SHADER);
        quadProg.load_from_file("res/shaders/quad_fs.glsl", gl::FRAGMENT_SHADER);
        quadProg.build();

        terrainProg.add_uniform("shadQuality", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        terrainProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        terrainProg.add_uniform("shadProjViewMat", sq::U_TYPE::u_4m);
        terrainProg.add_uniform("mapSize", sq::U_TYPE::u_2u);
        terrainProg.add_uniform("camPos", sq::U_TYPE::u_3f);
        terrainProg.add_uniform("skyLightDir", sq::U_TYPE::u_3f);
        terrainProg.add_uniform("skyLightDiff", sq::U_TYPE::u_3f);
        terrainProg.add_uniform("skyLightAmbi", sq::U_TYPE::u_3f);
        terrainProg.add_uniform("skyLightSpec", sq::U_TYPE::u_3f);
        terrainProg.add_uniform("texAmbi", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("texNormArray", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("texDiffArray", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("texSpecArray", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("texShad", sq::U_TYPE::u_1i);

        modelOpProg.add_uniform("shadQuality", sq::U_TYPE::u_1i);
        modelOpProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        modelOpProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        modelOpProg.add_uniform("modelMat", sq::U_TYPE::u_4m);
        modelOpProg.add_uniform("shadProjViewMat", sq::U_TYPE::u_4m);
        modelOpProg.add_uniform("camPos", sq::U_TYPE::u_3f);
        modelOpProg.add_uniform("skyLightDir", sq::U_TYPE::u_3f);
        modelOpProg.add_uniform("skyLightDiff", sq::U_TYPE::u_3f);
        modelOpProg.add_uniform("skyLightAmbi", sq::U_TYPE::u_3f);
        modelOpProg.add_uniform("skyLightSpec", sq::U_TYPE::u_3f);
        modelOpProg.add_uniform("texNorm", sq::U_TYPE::u_1i);
        modelOpProg.add_uniform("texDiff", sq::U_TYPE::u_1i);
        modelOpProg.add_uniform("texAmbi", sq::U_TYPE::u_1i);
        modelOpProg.add_uniform("texSpec", sq::U_TYPE::u_1i);
        modelOpProg.add_uniform("texShad", sq::U_TYPE::u_1i);

        shadowProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        shadowProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        shadowProg.add_uniform("modelMat", sq::U_TYPE::u_4m);

        lumaProg.add_uniform("screenTex", sq::U_TYPE::u_1i);

        fxaaLProg.add_uniform("screenTex", sq::U_TYPE::u_1i);
        fxaaLProg.add_uniform("vpPixSize", sq::U_TYPE::u_2f);
        fxaaHProg.add_uniform("screenTex", sq::U_TYPE::u_1i);
        fxaaHProg.add_uniform("vpPixSize", sq::U_TYPE::u_2f);

        quadProg.add_uniform("screenTex", sq::U_TYPE::u_1i);

        terrainProg.use();
        terrainProg.set_uniform_uv("mapSize", glm::value_ptr(level.size));
        terrainProg.set_uniform_i("texNormArray", 0);
        terrainProg.set_uniform_i("texDiffArray", 1);
        terrainProg.set_uniform_i("texAmbi", 2);
        terrainProg.set_uniform_i("texSpecArray", 3);
        terrainProg.set_uniform_i("texShad", 4);

        modelOpProg.use();
        modelOpProg.set_uniform_i("texNorm", 0);
        modelOpProg.set_uniform_i("texDiff", 1);
        modelOpProg.set_uniform_i("texAmbi", 2);
        modelOpProg.set_uniform_i("texSpec", 3);
        modelOpProg.set_uniform_i("texShad", 4);

        lumaProg.use();
        lumaProg.set_uniform_i("screenTex", 0);

        fxaaLProg.use();
        fxaaLProg.set_uniform_i("screenTex", 0);
        fxaaHProg.use();
        fxaaHProg.set_uniform_i("screenTex", 0);

        quadProg.use();
        quadProg.set_uniform_i("screenTex", 0);

        first = false;
    }

    if (modSettings) {
        if (settings.shad) {
            shadowFb.create(0, noDrawBuffers, true);
            if (settings.shad == 1)
                shadowFb.resize({1024, 1024}); // Low
            else if (settings.shad == 2)
                shadowFb.resize({2048, 2048}); // Medium
            else if (settings.shad == 3)
                shadowFb.resize({4096, 4096}); // High
            else if (settings.shad == 4)
                shadowFb.resize({8192, 8192}); // Extreme
            shadowFb.get(-1)->bind();
            shadowFb.get(-1)->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
            shadowFb.get(-1)->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        }
        terrainProg.use();
        terrainProg.set_uniform_i("shadQuality", settings.shad);
        modelOpProg.use();
        modelOpProg.set_uniform_i("shadQuality", settings.shad);

        aaLevel = settings.aa;
        shadLevel = settings.shad;

        modSettings = false;
    }

    if (updateFramebuffers) {
        glm::uvec2 size = app->get_size();
        gl::Viewport(0, 0, size.x, size.y);

        // Main Framebuffer
        mapFb.resize(size);
        mapFb.get(0)->bind();
        mapFb.get(0)->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);

        glm::vec2 pixSize = {1.f / float(size.x), 1.f / float(size.y)};
        fxaaLProg.use();
        fxaaLProg.set_uniform_fv("vpPixSize", glm::value_ptr(pixSize));
        fxaaHProg.use();
        fxaaHProg.set_uniform_fv("vpPixSize", glm::value_ptr(pixSize));

        updateFramebuffers = false;
    }

    if (updateSkyLight) {
        skyLight.update(skyLight.dir, level.size, 64.f);

        terrainProg.use();
        terrainProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        terrainProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        terrainProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        terrainProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));

        modelOpProg.use();
        modelOpProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        modelOpProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        modelOpProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        modelOpProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));

        updateSkyLight = false;
    }

    if (player.update_render(dt, accum)) {
        terrainProg.use();
        terrainProg.set_uniform_mv("projMat", false, glm::value_ptr(camera.projMat));
        terrainProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));
        terrainProg.set_uniform_mv("shadProjViewMat", false, glm::value_ptr(skyLight.projViewMat));
        terrainProg.set_uniform_fv("camPos", glm::value_ptr(camera.pos));

        modelOpProg.use();
        modelOpProg.set_uniform_mv("projMat", false, glm::value_ptr(camera.projMat));
        modelOpProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));
        modelOpProg.set_uniform_mv("shadProjViewMat", false, glm::value_ptr(skyLight.projViewMat));
        modelOpProg.set_uniform_fv("camPos", glm::value_ptr(camera.pos));

        shadowProg.use();
        shadowProg.set_uniform_mv("projMat", false, glm::value_ptr(skyLight.projMat));
        shadowProg.set_uniform_mv("viewMat", false, glm::value_ptr(skyLight.viewMat));
    }

    gl::Enable(gl::DEPTH_TEST);

    /// Shadow Texture ///
    if (shadLevel) {
        shadowFb.use();
        shadowFb.useVP();
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        shadowProg.use();
        shadowProg.set_uniform_mv("modelMat", false, glm::value_ptr(glm::mat4()));
        gl::BindVertexArray(level.terrain.vao);
        gl::DrawArrays(gl::TRIANGLES, 0, level.terrain.vCount);

        gl::BindVertexArray(player.model.mesh->vao);
        shadowProg.set_uniform_mv("modelMat", false, glm::value_ptr(player.modelMat));
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, player.model.mesh->ibo);
        gl::DrawElements(gl::TRIANGLES, player.model.mesh->iCount, gl::UNSIGNED_SHORT, 0);

        for (uint i = 0; i < level.modelVec.size(); i++) {
            gl::BindVertexArray(level.modelVec[i].mesh->vao);
            gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, level.modelVec[i].mesh->ibo);
            for (ModelInstance& m : level.modelInstVec) {
                if (m.index == i) {
                    shadowProg.set_uniform_mv("modelMat", false, glm::value_ptr(m.modelMat));
                    gl::DrawElements(gl::TRIANGLES, level.modelVec[m.index].mesh->iCount, gl::UNSIGNED_SHORT, 0);
                }
            }
        }
    }

    /// Map ///

    mapFb.use();
    mapFb.useVP();
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

    /// Ground ///
    terrainProg.use();

    level.terrain.texNormArray->bind(gl::TEXTURE0);
    level.terrain.texDiffArray->bind(gl::TEXTURE1);
    level.terrain.texAmbi->bind(gl::TEXTURE2);
    level.terrain.texSpecArray->bind(gl::TEXTURE3);
    if (shadLevel) shadowFb.get(-1)->bind(gl::TEXTURE4);

    gl::BindVertexArray(level.terrain.vao);
    gl::DrawArrays(gl::TRIANGLES, 0, level.terrain.vCount);


    /// Map Models Opaque ///
    modelOpProg.use();
    for (uint i = 0; i < level.modelVec.size(); i++) {
        if (level.modelVec[i].skin.alpha == true) continue;
        level.modelVec[i].skin.texNorm->bind(gl::TEXTURE0);
        level.modelVec[i].skin.texDiff->bind(gl::TEXTURE1);
        level.modelVec[i].skin.texSpec->bind(gl::TEXTURE3);
        gl::BindVertexArray(level.modelVec[i].mesh->vao);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, level.modelVec[i].mesh->ibo);
        for (ModelInstance& m : level.modelInstVec) {
            if (m.index == i) {
                modelOpProg.set_uniform_mv("modelMat", false, glm::value_ptr(m.modelMat));
                m.texAmbi->bind(gl::TEXTURE2);
                gl::DrawElements(gl::TRIANGLES, level.modelVec[m.index].mesh->iCount, gl::UNSIGNED_SHORT, 0);
            }
        }
    }


    /// Characters and Player ///
    gl::BindVertexArray(player.model.mesh->vao);
    player.model.skin.texNorm->bind(gl::TEXTURE0);
    player.model.skin.texDiff->bind(gl::TEXTURE1);
    player.model.skin.texSpec->bind(gl::TEXTURE3);
    modelOpProg.set_uniform_mv("modelMat", false, glm::value_ptr(player.modelMat));
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, player.model.mesh->ibo);
    gl::DrawElements(gl::TRIANGLES, player.model.mesh->iCount, gl::UNSIGNED_SHORT, 0);


    /// Map Models Translucent ///
    gl::Enable(gl::BLEND);
    for (uint i = 0; i < level.modelVec.size(); i++) {
        if (level.modelVec[i].skin.alpha == false) continue;
        level.modelVec[i].skin.texNorm->bind(gl::TEXTURE0);
        level.modelVec[i].skin.texDiff->bind(gl::TEXTURE1);
        level.modelVec[i].skin.texSpec->bind(gl::TEXTURE3);
        gl::BindVertexArray(level.modelVec[i].mesh->vao);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, level.modelVec[i].mesh->ibo);
        for (ModelInstance& m : level.modelInstVec) {
            if (m.index == i) {
                modelOpProg.set_uniform_mv("modelMat", false, glm::value_ptr(m.modelMat));
                m.texAmbi->bind(gl::TEXTURE2);
                gl::DrawElements(gl::TRIANGLES, level.modelVec[m.index].mesh->iCount, gl::UNSIGNED_SHORT, 0);
            }
        }
    }

    gl::Disable(gl::DEPTH_TEST);
    gl::Disable(gl::BLEND);
    if (aaLevel) { // Do FXAA
        /// Put Luma into Alpha ///
        lumaProg.use();
        mapFb.get(0)->bind(gl::TEXTURE0);
        screenQuad.draw();

        /// FXAA and screen output ///
        gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        if (aaLevel == 1) fxaaLProg.use();
        else if (aaLevel == 2) fxaaHProg.use();
        //shadowFb.get(-1)->bind();
        screenQuad.draw();
    } else { // No FXAA
        gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        quadProg.use();
        mapFb.get(0)->bind(gl::TEXTURE0);
        screenQuad.draw();
    }
}

bool HandlerGame::handle(sf::Event&) {
    return false;
}

}
