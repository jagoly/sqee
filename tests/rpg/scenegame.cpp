#include "scenegame.hpp"

#include <iostream>
#include <random>
#include <array>

#include <libsqee/app/application.hpp>
#include <libsqee/gl/shaders.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/gl/framebuffers.hpp>
#include <libsqee/text/font.hpp>
#include <libsqee/text/text.hpp>

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 24;
    dt = 1.d/24;

    level.set_holders(&meshH, &skinH, &texH);
    level.load_map("test");

    camera.init({0, 0, 0}, {0.375f, 0, 0}, 0, 0, 1.2f, 0.5f, 60.f);
    //camera.init({0, 0, 0}, {0, 0, 0}, 0, 0, 1.17f, 0.5f, 60.f);
    camera.update_projMat();
    camera.update_viewMat();
    camera.update_ubo();

    player.set_holders(&advMeshH, &texH);
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
    level.tick(tickRate);

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

    if (pendDir) tickTock++;
    else         tickTock--;
    if (std::abs(tickTock) == tickRate) pendDir = !pendDir;
    swingA = swingB;
    swingB = float(tickTock) / tickRate;
}

void SceneGame::render(float _ft) {
    static bool first = true;
    static glm::uvec2 size, margins, adjSize;

    static std::minstd_rand gen(std::time(nullptr)); gen();

    static sq::Framebuffer shadowFb;
    static sq::Framebuffer priFb;
    static sq::Framebuffer secFb;

    static sq::ScreenQuad screenQuad;

    static sq::Shader modelProg;
    static sq::Shader modelSubProg;
    static sq::Shader advmodelProg;
    static sq::Shader advmodelSubProg;
    static sq::Shader liquidProg;
    static sq::Shader mvpSaProg;
    static sq::Shader lumaProg;
    static sq::Shader fxaaLProg;
    static sq::Shader fxaaHProg;
    static sq::Shader quadProg;

    static sq::Font::Ptr font = sq::create_font("res/fonts/Ubuntu-R.ttf", app->ftLib, 48);
    static sq::TextHandles textHandles;

    if (first) {
        gl::DepthFunc(gl::LEQUAL);
        gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
        gl::ClearColor(0.1f, 0.05f, 0.2f, 1.f);

        level.load_objects();
        level.load_physics();
        priFb.create({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                     gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
        secFb.create({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                     gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);

        // Shaders
        modelProg.load_from_file("models/basic_vs", gl::VERTEX_SHADER);
        modelProg.load_from_file("models/basic_fs", gl::FRAGMENT_SHADER);
        modelProg.build();
        modelSubProg.load_from_file("models/basic_vs", gl::VERTEX_SHADER);
        modelSubProg.load_from_file("models/basicsub_fs", gl::FRAGMENT_SHADER);
        modelSubProg.build();
        advmodelProg.load_from_file("models/advanced_vs", gl::VERTEX_SHADER);
        advmodelProg.load_from_file("models/advanced_fs", gl::FRAGMENT_SHADER);
        advmodelProg.build();
        advmodelSubProg.load_from_file("models/advanced_vs", gl::VERTEX_SHADER);
        advmodelSubProg.load_from_file("models/advancedsub_fs", gl::FRAGMENT_SHADER);
        advmodelSubProg.build();
        liquidProg.load_from_file("liquids/planar_vs", gl::VERTEX_SHADER);
        liquidProg.load_from_file("liquids/planar_fs", gl::FRAGMENT_SHADER);
        liquidProg.build();
        mvpSaProg.load_from_file("generic/mvpsa_vs", gl::VERTEX_SHADER);
        mvpSaProg.load_from_file("generic/dummy_fs", gl::FRAGMENT_SHADER);
        mvpSaProg.build();
        lumaProg.load_from_file("generic/quad_vs", gl::VERTEX_SHADER);
        lumaProg.load_from_file("generic/luma_fs", gl::FRAGMENT_SHADER);
        lumaProg.build();
        fxaaLProg.load_from_file("generic/quad_vs", gl::VERTEX_SHADER);
        fxaaLProg.load_from_file("post/fxaal_fs", gl::FRAGMENT_SHADER);
        fxaaLProg.build();
        fxaaHProg.load_from_file("generic/quad_vs", gl::VERTEX_SHADER);
        fxaaHProg.load_from_file("post/fxaah_fs", gl::FRAGMENT_SHADER);
        fxaaHProg.build();
        quadProg.load_from_file("generic/quad_vs", gl::VERTEX_SHADER);
        quadProg.load_from_file("generic/passthrough_fs", gl::FRAGMENT_SHADER);
        quadProg.build();

        modelProg.add_uniform("shadQuality", sq::UType::u_1i);
        modelProg.add_uniform("modelMat", sq::UType::u_4m);
        modelProg.add_uniform("shadProjViewMat", sq::UType::u_4m);
        modelProg.add_uniform("skyLightDir", sq::UType::u_3f);
        modelProg.add_uniform("skyLightDiff", sq::UType::u_3f);
        modelProg.add_uniform("skyLightAmbi", sq::UType::u_3f);
        modelProg.add_uniform("skyLightSpec", sq::UType::u_3f);

        modelSubProg.add_uniform("shadQuality", sq::UType::u_1i);
        modelSubProg.add_uniform("modelMat", sq::UType::u_4m);
        modelSubProg.add_uniform("shadProjViewMat", sq::UType::u_4m);
        modelSubProg.add_uniform("skyLightDir", sq::UType::u_3f);
        modelSubProg.add_uniform("skyLightDiff", sq::UType::u_3f);
        modelSubProg.add_uniform("skyLightAmbi", sq::UType::u_3f);
        modelSubProg.add_uniform("skyLightSpec", sq::UType::u_3f);

        advmodelProg.add_uniform("shadQuality", sq::UType::u_1i);
        advmodelProg.add_uniform("modelMat", sq::UType::u_4m);
        advmodelProg.add_uniform("shadProjViewMat", sq::UType::u_4m);
        advmodelProg.add_uniform("skyLightDir", sq::UType::u_3f);
        advmodelProg.add_uniform("skyLightDiff", sq::UType::u_3f);
        advmodelProg.add_uniform("skyLightAmbi", sq::UType::u_3f);
        advmodelProg.add_uniform("skyLightSpec", sq::UType::u_3f);

        advmodelSubProg.add_uniform("shadQuality", sq::UType::u_1i);
        advmodelSubProg.add_uniform("modelMat", sq::UType::u_4m);
        advmodelSubProg.add_uniform("shadProjViewMat", sq::UType::u_4m);
        advmodelSubProg.add_uniform("skyLightDir", sq::UType::u_3f);
        advmodelSubProg.add_uniform("skyLightDiff", sq::UType::u_3f);
        advmodelSubProg.add_uniform("skyLightAmbi", sq::UType::u_3f);
        advmodelSubProg.add_uniform("skyLightSpec", sq::UType::u_3f);

        liquidProg.add_uniform("flowOffset", sq::UType::u_2f);
        liquidProg.add_uniform("scale", sq::UType::u_1f);
        liquidProg.add_uniform("skyLightDir", sq::UType::u_3f);
        liquidProg.add_uniform("skyLightSpec", sq::UType::u_3f);
        liquidProg.add_uniform("swing", sq::UType::u_1f);
        liquidProg.add_uniform("wSmooth", sq::UType::u_1f);
        liquidProg.add_uniform("tinge", sq::UType::u_3f);

        mvpSaProg.add_uniform("projMat", sq::UType::u_4m);
        mvpSaProg.add_uniform("viewMat", sq::UType::u_4m);
        mvpSaProg.add_uniform("modelMat", sq::UType::u_4m);

        fxaaLProg.add_uniform("vpPixSize", sq::UType::u_2f);
        fxaaHProg.add_uniform("vpPixSize", sq::UType::u_2f);

        texH.add("water_norms");
        texH.get("water_norms")->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8,
                                        glm::uvec3(1024, 1024, 2), sq::TexPreset::L_R);
        texH.get("water_norms")->buffer_file("static/water_norm1", 0);
        texH.get("water_norms")->buffer_file("static/water_norm2", 1);

        first = false;
    }

    if (modSettings) {
        if (settings.shad) {
            shadowFb.create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
            if (settings.shad == 1)
                shadowFb.resize({1024, 1024}); // Low
            else if (settings.shad == 2)
                shadowFb.resize({2048, 2048}); // Medium
            else if (settings.shad == 3)
                shadowFb.resize({4096, 4096}); // High
            else if (settings.shad == 4)
                shadowFb.resize({8192, 8192}); // Extreme
            shadowFb.dsTex.set_preset(sq::TexPreset::L_C);
            shadowFb.dsTex.set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        }
        modelProg.use();
        modelProg.set_uniform_i("shadQuality", settings.shad);
        modelSubProg.use();
        modelSubProg.set_uniform_i("shadQuality", settings.shad);

        advmodelProg.use();
        advmodelProg.set_uniform_i("shadQuality", settings.shad);
        advmodelSubProg.use();
        advmodelSubProg.set_uniform_i("shadQuality", settings.shad);

        modSettings = false;
    }

    if (updateFramebuffers) {
        size = app->get_size();
        margins = {size.x / 10, size.y / 10};
        adjSize = size + margins + margins;
        gl::Viewport(0, 0, size.x, size.y);

        // Main Framebuffer
        priFb.resize(size);
        priFb.cTexVec[0].set_preset(sq::TexPreset::L_C);

        // Liquid Depth Framebuffer
        secFb.resize(size);

        glm::vec2 pixSize = {1.f / float(size.x), 1.f / float(size.y)};
        fxaaLProg.use();
        fxaaLProg.set_uniform_fv("vpPixSize", glm::value_ptr(pixSize));
        fxaaHProg.use();
        fxaaHProg.set_uniform_fv("vpPixSize", glm::value_ptr(pixSize));

        updateFramebuffers = false;
    }

    if (updateSkyLight) {
        skyLight.update(skyLight.dir, level.size);

        modelProg.use();
        modelProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        modelProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        modelProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        modelProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));
        modelProg.set_uniform_mv("shadProjViewMat", false, glm::value_ptr(skyLight.projViewMat));

        modelSubProg.use();
        modelSubProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        modelSubProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        modelSubProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        modelSubProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));
        modelSubProg.set_uniform_mv("shadProjViewMat", false, glm::value_ptr(skyLight.projViewMat));

        advmodelProg.use();
        advmodelProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        advmodelProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        advmodelProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        advmodelProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));
        advmodelProg.set_uniform_mv("shadProjViewMat", false, glm::value_ptr(skyLight.projViewMat));

        advmodelSubProg.use();
        advmodelSubProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        advmodelSubProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        advmodelSubProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        advmodelSubProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));
        advmodelSubProg.set_uniform_mv("shadProjViewMat", false, glm::value_ptr(skyLight.projViewMat));

        liquidProg.use();
        liquidProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        liquidProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));

        mvpSaProg.use();
        mvpSaProg.set_uniform_mv("projMat", false, glm::value_ptr(skyLight.projMat));
        mvpSaProg.set_uniform_mv("viewMat", false, glm::value_ptr(skyLight.viewMat));

        updateSkyLight = false;
    }

    player.update_render(dt, accum);
    level.update_render(_ft);


    /// Shadow Texture ///////////////////////////////////

    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);

    if (settings.shad) {
        mvpSaProg.use();
        mvpSaProg.set_uniform_mv("projMat", false, glm::value_ptr(skyLight.projMat));
        mvpSaProg.set_uniform_mv("viewMat", false, glm::value_ptr(skyLight.viewMat));
        shadowFb.use();
        shadowFb.useVP();
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        mvpSaProg.use();

        for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bop : level.objectMap) {
            if (bop.second->type == obj::Type::Model) {
                obj::Model* o = static_cast<obj::Model*>(bop.second.get());
                if (!o->shad) continue;
                mvpSaProg.set_uniform_mv("modelMat", false, glm::value_ptr(o->modelMat));

                o->mesh->bind_buffers();
                gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
            }
        }
        // PLAYER MODEL TODO

        shadowFb.dsTex.bind(gl::TEXTURE4);
    }


    /// Submerged Areas ///

    priFb.useVP();
    priFb.use();
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    static glm::mat4 iMat;
    mvpSaProg.use();
    mvpSaProg.set_uniform_mv("projMat", false, glm::value_ptr(camera.projMat));
    mvpSaProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));
    mvpSaProg.set_uniform_mv("modelMat", false, glm::value_ptr(iMat));

    for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo : level.objectMap) {
        if (bo.second->type == obj::Type::Liquid) {
            obj::Liquid* o = static_cast<obj::Liquid*>(bo.second.get());
            gl::BindVertexArray(o->vao);
            gl::DrawArrays(gl::TRIANGLES, 0, 6);
        }
    }

    secFb.use();
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
    modelSubProg.use();
    priFb.dsTex.bind(gl::TEXTURE5);
    for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo : level.objectMap) {
        if (bo.second->type == obj::Type::Model) {
            obj::Model* o = static_cast<obj::Model*>(bo.second.get());
            if (!o->subm) continue;

            o->mesh->bind_buffers();
            o->skin->bind_textures();
            modelSubProg.set_uniform_mv("modelMat", false, glm::value_ptr(o->modelMat));

            gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
        }
    }

    advmodelSubProg.use();
    player.mesh->bind_buffers();
    advmodelSubProg.set_uniform_mv("modelMat", false, glm::value_ptr(player.modelMat));
    gl::DrawElements(gl::TRIANGLES, player.mesh->iCount, gl::UNSIGNED_INT, 0);


    /// Liquids ///

    priFb.use();
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
    liquidProg.use();
    texH.get("water_norms")->bind(gl::TEXTURE0);
    secFb.cTexVec[0].bind(gl::TEXTURE1);
    for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo : level.objectMap) {
        if (bo.second->type == obj::Type::Liquid) {
            obj::Liquid* o = static_cast<obj::Liquid*>(bo.second.get());

            glm::vec2 offset = o->flowOffsetA * float(((dt - accum) / dt)) +
                               o->flowOffsetB * float((accum / dt));
            liquidProg.set_uniform_fv("flowOffset", glm::value_ptr(offset));
            liquidProg.set_uniform_f("scale", o->scale);
            liquidProg.set_uniform_f("wSmooth", o->wSmooth);
            liquidProg.set_uniform_f("swing", swingA * ((dt - accum) / dt) + swingB * (accum / dt));
            liquidProg.set_uniform_fv("tinge", glm::value_ptr(o->tinge));
            gl::BindVertexArray(o->vao);
            gl::DrawArrays(gl::TRIANGLES, 0, 6);
        }
    }

    /// Characters and Player ///
    advmodelProg.use();
    player.mesh->bind_buffers();
    advmodelProg.set_uniform_mv("modelMat", false, glm::value_ptr(player.modelMat));
    gl::DrawElements(gl::TRIANGLES, player.mesh->iCount, gl::UNSIGNED_INT, 0);


    /// Map Models ///

    /// Map Models Opaque ///
    modelProg.use();
    for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo : level.objectMap) {
        if (bo.second->type == obj::Type::Model) {
            obj::Model* o = static_cast<obj::Model*>(bo.second.get());
            if (o->skin->alpha) continue;

            o->mesh->bind_buffers();
            o->skin->bind_textures();
            modelProg.set_uniform_mv("modelMat", false, glm::value_ptr(o->modelMat));

            gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
        }
    }


    /// Map Models Translucent ///
    gl::Enable(gl::BLEND);
    for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo : level.objectMap) {
        if (bo.second->type == obj::Type::Model) {
            obj::Model* o = static_cast<obj::Model*>(bo.second.get());
            if (!o->skin->alpha) continue;

            o->mesh->bind_buffers();
            o->skin->bind_textures();
            modelProg.set_uniform_mv("modelMat", false, glm::value_ptr(o->modelMat));

            gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
        }
    }
    gl::Disable(gl::BLEND);


    /// FXAA ///
    gl::Disable(gl::DEPTH_TEST);
    if (settings.aa) { // Do FXAA
        /// Put Luma into Alpha ///
        lumaProg.use();
        priFb.cTexVec[0].bind(gl::TEXTURE0);
        screenQuad.draw();

        /// FXAA and screen output ///
        gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        if (settings.aa == 1) fxaaLProg.use();
        else if (settings.aa == 2) fxaaHProg.use();
        screenQuad.draw();
    } else { // No FXAA
        gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        quadProg.use();
        priFb.cTexVec[0].bind(gl::TEXTURE0);
        screenQuad.draw();
    }

    gl::Viewport(0, 0, size.x, size.y);
    gl::Enable(gl::BLEND);

    static float fps = 60.f;
    fps = fps * 0.9f + 1.f / _ft * 0.1f;
    sq::draw_text(textHandles, font, "FPS: "+std::to_string(int(fps)), {16, size.y - 32}, size);
}

bool HandlerGame::handle(sf::Event&) {
    return false;
}

}
