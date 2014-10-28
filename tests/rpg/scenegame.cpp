#include <iostream>
#include <random>
#include <array>
#include <list>

#include <sqee/app/application.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/framebuffers.hpp>

#include "scenegame.hpp"

using namespace sqt;

SceneGame::SceneGame(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 24;

    level = Level::Ptr(new Level(meshH, skinH, texH));
    level->load_map("test");

    camera.init({0, 0, 0}, {0.37f, 0, 0}, 0, 0, 1.2f, 0.5f, 60.f);
    //camera.init({0, 0, 0}, {0, 0, 0}, 0, 0, 1.2f, 1.f, 20.f);
    camera.update_projMat();
    camera.update_viewMat();
    camera.update_ubo();

    player = Player::Ptr(new Player(advMeshH, advSkinH, texH));
    player->test_init();

    resize(app.get_size());
}

void SceneGame::resize(glm::uvec2 _size) {
    camera.width = _size.x;
    camera.height = _size.y;
    updateFramebuffers = true;
}

void SceneGame::update() {
    level->tick();

    static std::list<char> keys = {-1};
    static std::array<bool, 4> keyStates = {false, false, false, false};
    std::array<bool, 4> keyStatesPrev = keyStates;

    keyStates[0] = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    keyStates[1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    keyStates[2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    keyStates[3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);

    for (int i = 0; i < 4; i++) {
        if (keyStates[i] && !keyStatesPrev[i]) {
            keys.remove(i);
            keys.emplace_front(i);
        } else
        if (!keyStates[i] && keyStatesPrev[i]) {
            keys.remove(i);
        }
    }

    player->tick(keys.front(), level);

    if (pendDir) tickTock++;
    else         tickTock--;
    if (std::abs(tickTock) == tickRate) pendDir = !pendDir;
    swingA = swingB;
    swingB = float(tickTock) / tickRate;
}

void SceneGame::render(float _ft) {
    const double dt = 1.d / 24.d;

    static bool first = true;

    static glm::uvec2 size;

    static sq::Framebuffer priFb;
    static sq::Framebuffer secFb;
    static sq::Framebuffer terFb;
    static sq::Framebuffer shadowFb;

    static sq::ScreenQuad screenQuad;

    static sq::Shader modelProg;
    static sq::Shader advmodelProg;
    static sq::Shader liquidProg;
    static sq::Shader mvpProg;
    static sq::Shader mvpskelProg;
    static sq::Shader lumaProg;
    static sq::Shader fxaaLProg;
    static sq::Shader fxaaHProg;
    static sq::Shader quadProg;


    /// Set things up on first call ///

    if (first) {
        level->load_objects();
        level->load_physics();

        priFb.create({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                     gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
        secFb.create({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                     gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
        terFb.create({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                     gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);

        modelProg.load_from_file("models/basic_vs", gl::VERTEX_SHADER);
        modelProg.load_from_file("models/basic_fs", gl::FRAGMENT_SHADER);
        modelProg.build();
        advmodelProg.load_from_file("models/advanced_vs", gl::VERTEX_SHADER);
        advmodelProg.load_from_file("models/advanced_fs", gl::FRAGMENT_SHADER);
        advmodelProg.build();
        liquidProg.load_from_file("liquids/surface_vs", gl::VERTEX_SHADER);
        liquidProg.load_from_file("liquids/surface_fs", gl::FRAGMENT_SHADER);
        liquidProg.build();
        mvpProg.load_from_file("generic/mvp_vs", gl::VERTEX_SHADER);
        mvpProg.load_from_file("generic/dummy_fs", gl::FRAGMENT_SHADER);
        mvpProg.build();
        mvpskelProg.load_from_file("generic/mvpskel_vs", gl::VERTEX_SHADER);
        mvpskelProg.load_from_file("generic/dummy_fs", gl::FRAGMENT_SHADER);
        mvpskelProg.build();
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

        modelProg.add_uniform("shadQuality", sq::UType::i1);
        modelProg.add_uniform("reflMat", sq::UType::m4);
        modelProg.add_uniform("clipZ", sq::UType::f1);
        modelProg.add_uniform("modelMat", sq::UType::m4);
        modelProg.add_uniform("shadProjViewMat", sq::UType::m4);
        modelProg.add_uniform("skyLightDir", sq::UType::f3);
        modelProg.add_uniform("skyLightDiff", sq::UType::f3);
        modelProg.add_uniform("skyLightAmbi", sq::UType::f3);
        modelProg.add_uniform("skyLightSpec", sq::UType::f3);

        advmodelProg.add_uniform("shadQuality", sq::UType::i1);
        advmodelProg.add_uniform("reflMat", sq::UType::m4);
        advmodelProg.add_uniform("clipZ", sq::UType::f1);
        advmodelProg.add_uniform_v("s_quat", 40, sq::UType::f4);
        advmodelProg.add_uniform_v("s_offs", 40, sq::UType::f3);
        advmodelProg.add_uniform("modelMat", sq::UType::m4);
        advmodelProg.add_uniform("shadProjViewMat", sq::UType::m4);
        advmodelProg.add_uniform("skyLightDir", sq::UType::f3);
        advmodelProg.add_uniform("skyLightDiff", sq::UType::f3);
        advmodelProg.add_uniform("skyLightAmbi", sq::UType::f3);
        advmodelProg.add_uniform("skyLightSpec", sq::UType::f3);

        liquidProg.add_uniform("reflMat", sq::UType::m4);
        liquidProg.add_uniform("zPos", sq::UType::f1);
        liquidProg.add_uniform("flowOffset", sq::UType::f2);
        liquidProg.add_uniform("scale", sq::UType::f1);
        liquidProg.add_uniform("skyLightDir", sq::UType::f3);
        liquidProg.add_uniform("skyLightSpec", sq::UType::f3);
        liquidProg.add_uniform("swing", sq::UType::f1);
        liquidProg.add_uniform("wSmooth", sq::UType::f1);

        mvpProg.add_uniform("projMat", sq::UType::m4);
        mvpProg.add_uniform("viewMat", sq::UType::m4);
        mvpProg.add_uniform("modelMat", sq::UType::m4);

        mvpskelProg.add_uniform("projMat", sq::UType::m4);
        mvpskelProg.add_uniform("viewMat", sq::UType::m4);
        mvpskelProg.add_uniform("modelMat", sq::UType::m4);
        mvpskelProg.add_uniform_v("s_quat", 40, sq::UType::f4);
        mvpskelProg.add_uniform_v("s_offs", 40, sq::UType::f3);

        fxaaLProg.add_uniform("vpPixSize", sq::UType::f2);
        fxaaHProg.add_uniform("vpPixSize", sq::UType::f2);

        texH.add("water_norms");
        texH.get("water_norms")->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_R);
        texH.get("water_norms")->resize({1024, 1024, 2});
        texH.get("water_norms")->buffer_file("static/water_norm1", 0);
        texH.get("water_norms")->buffer_file("static/water_norm2", 1);

        first = false;
    }


    /// Act Upon Changed Settings ///

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

        advmodelProg.use();
        advmodelProg.set_uniform_i("shadQuality", settings.shad);

        modSettings = false;
    }


    /// Act Upon Changed Framebuffers ///

    if (updateFramebuffers) {
        size = app.get_size();
        gl::Viewport(0, 0, size.x, size.y);

        // Primary Framebuffer
        priFb.resize(size);
        priFb.cTexVec[0].set_preset(sq::TexPreset::L_C);

        // Secondary Framebuffer
        secFb.resize(size);
        secFb.cTexVec[0].set_preset(sq::TexPreset::L_C);

        // Tertiary Framebuffer
        terFb.resize(size);
        terFb.cTexVec[0].set_preset(sq::TexPreset::L_C);

        glm::vec2 pixSize = {1.f / float(size.x), 1.f / float(size.y)};
        fxaaLProg.use();
        fxaaLProg.set_uniform_fv("vpPixSize", glm::value_ptr(pixSize));
        fxaaHProg.use();
        fxaaHProg.set_uniform_fv("vpPixSize", glm::value_ptr(pixSize));

        updateFramebuffers = false;
    }


    /// Act Upon Changed Skylight ///

    if (updateSkyLight) {
        skyLight.update(skyLight.dir, level->size);

        modelProg.use();
        modelProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        modelProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        modelProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        modelProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));
        modelProg.set_uniform_mv("shadProjViewMat", false, glm::value_ptr(skyLight.projViewMat));

        advmodelProg.use();
        advmodelProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        advmodelProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        advmodelProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        advmodelProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));
        advmodelProg.set_uniform_mv("shadProjViewMat", false, glm::value_ptr(skyLight.projViewMat));

        liquidProg.use();
        liquidProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        liquidProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));

        updateSkyLight = false;
    }

    gl::DepthFunc(gl::LEQUAL);
    gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
    gl::ClearColor(0.f, 0.f, 0.f, 1.f);
    gl::ClearDepth(1.d);

    player->calc(accum, camera);
    level->calc(accum);


    /// Shadow Texture ///

    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    gl::Enable(gl::CULL_FACE);

    if (settings.shad) {
        shadowFb.use();
        shadowFb.useVP();
        shadowFb.clear();

        mvpProg.use();
        mvpProg.set_uniform_mv("projMat", false, glm::value_ptr(skyLight.projMat));
        mvpProg.set_uniform_mv("viewMat", false, glm::value_ptr(skyLight.viewMat));
        for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bop : level->objectMap) {
            if (bop.second->type == obj::Type::Model) {
                obj::Model* o = static_cast<obj::Model*>(bop.second.get());
                if (!o->shad) continue;
                mvpProg.set_uniform_mv("modelMat", false, glm::value_ptr(o->modelMat));

                o->mesh->bind_buffers();
                gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
            }
        }

        mvpskelProg.use();
        mvpskelProg.set_uniform_mv("projMat", false, glm::value_ptr(skyLight.projMat));
        mvpskelProg.set_uniform_mv("viewMat", false, glm::value_ptr(skyLight.viewMat));
        player->mesh->bind_buffers();
        mvpskelProg.set_uniform_mv("modelMat", false, glm::value_ptr(player->modelMat));
        mvpskelProg.set_uniform_fv("s_quat", player->skel->pose.quatData);
        mvpskelProg.set_uniform_fv("s_offs", player->skel->pose.offsData);
        gl::DrawElements(gl::TRIANGLES, player->mesh->iCount, gl::UNSIGNED_INT, 0);

        shadowFb.dsTex.bind(gl::TEXTURE4);
    }


    /// Liquids and submerged areas ///

    priFb.useVP();
    priFb.clear();

    for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo : level->objectMap) {
        if (bo.second->type != obj::Type::Liquid) continue;
        obj::Liquid* o = static_cast<obj::Liquid*>(bo.second.get());

        gl::Enable(gl::BLEND);
        modelProg.use();
        modelProg.set_uniform_f("clipZ", o->zPos);
        advmodelProg.use();
        advmodelProg.set_uniform_mv("reflMat", false, glm::value_ptr(o->reflMat));
        advmodelProg.set_uniform_f("clipZ", o->zPos);

        //////////////////////////////////////////

        gl::Enable(gl::CLIP_DISTANCE0);
        gl::CullFace(gl::FRONT);

        secFb.clear({0.5f, 0.5f, 0.7f, 1.f});
        modelProg.use();
        modelProg.set_uniform_mv("reflMat", false, glm::value_ptr(o->reflMat));
        for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo2 : level->objectMap) {
            if (bo2.second->type != obj::Type::Model) continue;
            obj::Model* o2 = static_cast<obj::Model*>(bo2.second.get());
            if (!o2->refl) continue;

            o2->mesh->bind_buffers();
            o2->skin->bind_textures();
            modelProg.set_uniform_mv("modelMat", false, glm::value_ptr(o2->modelMat));

            gl::DrawElements(gl::TRIANGLES, o2->mesh->iCount, gl::UNSIGNED_INT, 0);
        }

        advmodelProg.use();
        advmodelProg.set_uniform_mv("reflMat", false, glm::value_ptr(o->reflMat));
        player->mesh->bind_buffers();
        player->skin->bind_textures();
        advmodelProg.set_uniform_mv("modelMat", false, glm::value_ptr(player->modelMat));
        advmodelProg.set_uniform_fv("s_quat", player->skel->pose.quatData);
        advmodelProg.set_uniform_fv("s_offs", player->skel->pose.offsData);
        gl::DrawElements(gl::TRIANGLES, player->mesh->iCount, gl::UNSIGNED_INT, 0);

        gl::Disable(gl::CLIP_DISTANCE0);
        gl::CullFace(gl::BACK);

        ////////////////////////////////////////

        gl::Enable(gl::CLIP_DISTANCE1);

        terFb.clear();
        modelProg.use();
        modelProg.set_uniform_mv("reflMat", false, glm::value_ptr(sq::iMat4));
        for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo2 : level->objectMap) {
            if (bo2.second->type != obj::Type::Model) continue;
            obj::Model* o2 = static_cast<obj::Model*>(bo2.second.get());
            if (!o2->refl) continue;

            o2->mesh->bind_buffers();
            o2->skin->bind_textures();
            modelProg.set_uniform_mv("modelMat", false, glm::value_ptr(o2->modelMat));
            gl::DrawElements(gl::TRIANGLES, o2->mesh->iCount, gl::UNSIGNED_INT, 0);
        }

        advmodelProg.use();
        advmodelProg.set_uniform_mv("reflMat", false, glm::value_ptr(sq::iMat4));
        player->mesh->bind_buffers();
        player->skin->bind_textures();
        advmodelProg.set_uniform_mv("modelMat", false, glm::value_ptr(player->modelMat));
        advmodelProg.set_uniform_fv("s_quat", player->skel->pose.quatData);
        advmodelProg.set_uniform_fv("s_offs", player->skel->pose.offsData);
        gl::DrawElements(gl::TRIANGLES, player->mesh->iCount, gl::UNSIGNED_INT, 0);

        gl::Disable(gl::CLIP_DISTANCE1);

        ////////////////////////////////////////

        gl::Disable(gl::BLEND);

        priFb.use();
        liquidProg.use();
        secFb.cTexVec[0].bind(gl::TEXTURE0);
        terFb.cTexVec[0].bind(gl::TEXTURE1);
        terFb.dsTex.bind(gl::TEXTURE2);
        texH.get("water_norms")->bind(gl::TEXTURE3);
        glm::vec2 offset = glm::mix(o->flowOffsetA, o->flowOffsetB, accum / dt);
        liquidProg.set_uniform_mv("reflMat", false, glm::value_ptr(o->reflMat));
        liquidProg.set_uniform_f("zPos", o->zPos);
        liquidProg.set_uniform_fv("flowOffset", glm::value_ptr(offset));
        liquidProg.set_uniform_f("scale", o->scale);
        liquidProg.set_uniform_f("wSmooth", o->wSmooth);
        liquidProg.set_uniform_f("swing", glm::mix(swingA, swingB, accum / dt));
        gl::BindVertexArray(o->vao);
        gl::DrawArrays(gl::TRIANGLES, 0, 6);
    }


    /// Characters and Player ///

    advmodelProg.use();
    advmodelProg.set_uniform_mv("reflMat", false, glm::value_ptr(sq::iMat4));
    player->mesh->bind_buffers();
    player->skin->bind_textures();
    advmodelProg.set_uniform_mv("modelMat", false, glm::value_ptr(player->modelMat));
    advmodelProg.set_uniform_fv("s_quat", player->skel->pose.quatData);
    advmodelProg.set_uniform_fv("s_offs", player->skel->pose.offsData);
    gl::DrawElements(gl::TRIANGLES, player->mesh->iCount, gl::UNSIGNED_INT, 0);


    /// Map Models Opaque ///

    modelProg.use();
    modelProg.set_uniform_mv("reflMat", false, glm::value_ptr(sq::iMat4));
    for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo : level->objectMap) {
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
    for (std::pair<const std::string, std::unique_ptr<obj::Object>>& bo : level->objectMap) {
        if (bo.second->type == obj::Type::Model) {
            obj::Model* o = static_cast<obj::Model*>(bo.second.get());
            if (!o->skin->alpha) continue;

            o->mesh->bind_buffers();
            o->skin->bind_textures();
            modelProg.set_uniform_mv("modelMat", false, glm::value_ptr(o->modelMat));

            gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
        }
    }


    /// FXAA and Screen Output ///

    gl::Disable(gl::BLEND);
    gl::Disable(gl::DEPTH_TEST);
    if (settings.aa) {
        lumaProg.use();
        priFb.cTexVec[0].bind(gl::TEXTURE0);
        screenQuad.draw();

        gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        if (settings.aa == 1) fxaaLProg.use();
        else if (settings.aa == 2) fxaaHProg.use();
        screenQuad.draw();
    } else {
        gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        quadProg.use();
        priFb.cTexVec[0].bind(gl::TEXTURE0);
        screenQuad.draw();
    }
}


bool HandlerGame::handle(const sf::Event&) {
    return false;
}
