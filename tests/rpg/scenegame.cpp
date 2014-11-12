#include <iostream>
#include <random>

#include <sqee/app/application.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/framebuffers.hpp>

#include "scenegame.hpp"

using namespace sqt;

SceneGame::SceneGame(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 24;

    world.load_base("WorldTest");

    camera.init({0, 0, 0}, {0.37f, 0, 0}, 0, 0, 1.2f, 0.5f, 60.f);
    //camera.init({0, 0, 0}, {0, 0, 0}, 0, 0, 1.2f, 1.f, 20.f);
    camera.update_projMat();
    camera.update_viewMat();
    camera.update_ubo();

    player.test_init();

    resize(app.get_size());
}

void SceneGame::resize(glm::uvec2 _size) {
    camera.width = _size.x;
    camera.height = _size.y;
    updateFramebuffers = true;
}

void SceneGame::update() {
    world.tick();

    static list<char> keys = {-1};
    static array<bool, 4> keyStates = {false, false, false, false};
    array<bool, 4> keyStatesPrev = keyStates;

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

    player.tick(keys.front(), world);

    if (pendDir) tickTock++;
    else         tickTock--;
    if (std::abs(tickTock) == tickRate) pendDir = !pendDir;
    swingA = swingB;
    swingB = float(tickTock) / tickRate;
}

void SceneGame::render(float _ft) {
    const double dt = 1.d / 24.d;

    static bool first = true;

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

        modelProg.add_uniform("shadQuality");
        modelProg.add_uniform("reflMat");
        modelProg.add_uniform("useRefl");
        modelProg.add_uniform("clipZ");
        modelProg.add_uniform("modelMat");
        modelProg.add_uniform("shadProjViewMat");
        modelProg.add_uniform("skyLightDir");
        modelProg.add_uniform("skyLightDiff");
        modelProg.add_uniform("skyLightAmbi");
        modelProg.add_uniform("skyLightSpec");

        advmodelProg.add_uniform("shadQuality");
        advmodelProg.add_uniform("reflMat");
        advmodelProg.add_uniform("useRefl");
        advmodelProg.add_uniform("clipZ");
        advmodelProg.add_uniform("s_quat", 40);
        advmodelProg.add_uniform("s_offs", 40);
        advmodelProg.add_uniform("modelMat");
        advmodelProg.add_uniform("shadProjViewMat");
        advmodelProg.add_uniform("skyLightDir");
        advmodelProg.add_uniform("skyLightDiff");
        advmodelProg.add_uniform("skyLightAmbi");
        advmodelProg.add_uniform("skyLightSpec");

        liquidProg.add_uniform("reflMat");
        liquidProg.add_uniform("translation");
        liquidProg.add_uniform("flowOffset");
        liquidProg.add_uniform("scale");
        liquidProg.add_uniform("skyLightDir");
        liquidProg.add_uniform("skyLightSpec");
        liquidProg.add_uniform("swing");
        liquidProg.add_uniform("wSmooth");

        mvpProg.add_uniform("projMat");
        mvpProg.add_uniform("viewMat");
        mvpProg.add_uniform("modelMat");

        mvpskelProg.add_uniform("projMat");
        mvpskelProg.add_uniform("viewMat");
        mvpskelProg.add_uniform("modelMat");
        mvpskelProg.add_uniform("s_quat", 40);
        mvpskelProg.add_uniform("s_offs", 40);

        fxaaLProg.add_uniform("vpPixSize");
        fxaaHProg.add_uniform("vpPixSize");

        resBank().texH.add("water_norms");
        resBank().texH.get("water_norms")->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_R);
        resBank().texH.get("water_norms")->resize({1024, 1024, 2});
        resBank().texH.get("water_norms")->buffer_file("static/water_norm1", 0);
        resBank().texH.get("water_norms")->buffer_file("static/water_norm2", 1);

        first = false;
    }


    /// Act Upon Changed Settings ///

    if (modSettings) {
        int shad = settings.get_int("shadows");
        if (shad) {
            shadowFb.create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
            if (shad == 1)
                shadowFb.resize({1024, 1024}); // Low
            else if (shad == 2)
                shadowFb.resize({2048, 2048}); // Medium
            else if (shad == 3)
                shadowFb.resize({4096, 4096}); // High
            else if (shad == 4)
                shadowFb.resize({8192, 8192}); // Extreme
            shadowFb.dsTex.set_preset(sq::TexPreset::L_C);
            shadowFb.dsTex.set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        }
        modelProg.use();
        modelProg.set_I("shadQuality", shad);

        advmodelProg.use();
        advmodelProg.set_I("shadQuality", shad);

        modSettings = false;
    }


    /// Act Upon Changed Framebuffers ///

    if (updateFramebuffers) {
        glm::uvec2 size = app.get_size();
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
        fxaaLProg.set_glmvec("vpPixSize", pixSize);
        fxaaHProg.use();
        fxaaHProg.set_glmvec("vpPixSize", pixSize);

        updateFramebuffers = false;
    }


    /// Act Upon Changed Skylight ///

    if (updateSkyLight) {
        skyLight.update(skyLight.dir, world.minPos, world.maxPos);

        modelProg.use();
        modelProg.set_glmvec("skyLightDir", skyLight.dir);
        modelProg.set_glmvec("skyLightDiff", skyLight.diff);
        modelProg.set_glmvec("skyLightAmbi", skyLight.ambi);
        modelProg.set_glmvec("skyLightSpec", skyLight.spec);
        modelProg.set_glmmat("shadProjViewMat", skyLight.projViewMat);

        advmodelProg.use();
        advmodelProg.set_glmvec("skyLightDir", skyLight.dir);
        advmodelProg.set_glmvec("skyLightDiff", skyLight.diff);
        advmodelProg.set_glmvec("skyLightAmbi", skyLight.ambi);
        advmodelProg.set_glmvec("skyLightSpec", skyLight.spec);
        advmodelProg.set_glmmat("shadProjViewMat", skyLight.projViewMat);

        liquidProg.use();
        liquidProg.set_glmvec("skyLightDir", skyLight.dir);
        liquidProg.set_glmvec("skyLightSpec", skyLight.spec);

        updateSkyLight = false;
    }

    gl::DepthFunc(gl::LEQUAL);
    gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
    gl::ClearColor(0.f, 0.f, 0.f, 1.f);
    gl::ClearDepth(1.d);

    player.calc(accum, camera);
    world.calc(accum);


    /// Shadow Texture ///

    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    gl::Enable(gl::CULL_FACE);

    if (settings.get_int("shadows")) {
        shadowFb.useVP();
        shadowFb.clear();

        mvpProg.use();
        mvpProg.set_glmmat("projMat", skyLight.projMat);
        mvpProg.set_glmmat("viewMat", skyLight.viewMat);
        for (wld::World::OPair& bop : world.cellObjects) {
            if (bop.second->type == wld::ObjType::Model) {
                wld::Model* o = static_cast<wld::Model*>(bop.second);
                if (!o->shad) continue;
                mvpProg.set_glmmat("modelMat", glm::translate(o->modelMat, bop.first));

                o->mesh->bind_buffers();
                gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
            }
        }

        mvpskelProg.use();
        mvpskelProg.set_glmmat("projMat", skyLight.projMat);
        mvpskelProg.set_glmmat("viewMat", skyLight.viewMat);
        player.model.mesh->bind_buffers(gl::TEXTURE3);
        mvpskelProg.set_glmmat("modelMat", player.model.modelMat);
        mvpskelProg.set_ptrvec<glm::vec4>("s_quat", player.model.skeleton.quatData);
        mvpskelProg.set_ptrvec<glm::vec3>("s_offs", player.model.skeleton.offsData);
        gl::DrawElements(gl::TRIANGLES, player.model.mesh->iCount, gl::UNSIGNED_INT, 0);

        shadowFb.dsTex.bind(gl::TEXTURE4);
    }


    /// Liquids and submerged areas ///

    priFb.useVP();
    priFb.clear();

    for (wld::World::OPair& bop : world.cellObjects) {
        if (bop.second->type != wld::ObjType::Liquid) continue;
        wld::Liquid* o = static_cast<wld::Liquid*>(bop.second);

        gl::Enable(gl::BLEND);
        modelProg.use();
        modelProg.set_glmmat("reflMat", o->reflMat);
        modelProg.set_F("clipZ", o->pos.z);
        advmodelProg.use();
        advmodelProg.set_glmmat("reflMat", o->reflMat);
        advmodelProg.set_F("clipZ", o->pos.z);

        //////////////////////////////////////////

        gl::Enable(gl::CLIP_DISTANCE0);
        gl::CullFace(gl::FRONT);

        secFb.clear({0.5f, 0.5f, 0.7f, 1.f});
        modelProg.use();
        modelProg.set_I("useRefl", 1);
        for (wld::World::OPair& bop2 : world.cellObjects) {
            if (bop2.second->type != wld::ObjType::Model) continue;
            wld::Model* o2 = static_cast<wld::Model*>(bop2.second);
            if (!o2->refl) continue;

            o2->mesh->bind_buffers();
            o2->bind_textures();
            modelProg.set_glmmat("modelMat", glm::translate(o2->modelMat, bop2.first));

            gl::DrawElements(gl::TRIANGLES, o2->mesh->iCount, gl::UNSIGNED_INT, 0);
        }

        advmodelProg.use();
        advmodelProg.set_I("useRefl", 1);
        player.model.mesh->bind_buffers(gl::TEXTURE3);
        player.model.bind_textures();
        advmodelProg.set_glmmat("modelMat", player.model.modelMat);
        advmodelProg.set_ptrvec<glm::vec4>("s_quat", player.model.skeleton.quatData);
        advmodelProg.set_ptrvec<glm::vec3>("s_offs", player.model.skeleton.offsData);
        gl::DrawElements(gl::TRIANGLES, player.model.mesh->iCount, gl::UNSIGNED_INT, 0);

        gl::Disable(gl::CLIP_DISTANCE0);
        gl::CullFace(gl::BACK);

        ////////////////////////////////////////

        gl::Enable(gl::CLIP_DISTANCE1);

        terFb.clear();
        modelProg.use();
        modelProg.set_I("useRefl", 0);
        for (wld::World::OPair& bop2 : world.cellObjects) {
            if (bop2.second->type != wld::ObjType::Model) continue;
            wld::Model* o2 = static_cast<wld::Model*>(bop2.second);
            if (!o2->refl) continue;

            o2->mesh->bind_buffers();
            o2->bind_textures();
            modelProg.set_glmmat("modelMat", glm::translate(o2->modelMat, bop2.first));
            gl::DrawElements(gl::TRIANGLES, o2->mesh->iCount, gl::UNSIGNED_INT, 0);
        }

        advmodelProg.use();
        advmodelProg.set_I("useRefl", 0);
        player.model.mesh->bind_buffers(gl::TEXTURE3);
        player.model.bind_textures();
        advmodelProg.set_glmmat("modelMat", player.model.modelMat);
        advmodelProg.set_ptrvec<glm::vec4>("s_quat", player.model.skeleton.quatData);
        advmodelProg.set_ptrvec<glm::vec3>("s_offs", player.model.skeleton.offsData);
        gl::DrawElements(gl::TRIANGLES, player.model.mesh->iCount, gl::UNSIGNED_INT, 0);

        gl::Disable(gl::CLIP_DISTANCE1);

        ////////////////////////////////////////

        gl::Disable(gl::BLEND);

        priFb.use();
        liquidProg.use();
        secFb.cTexVec[0].bind(gl::TEXTURE0);
        terFb.cTexVec[0].bind(gl::TEXTURE1);
        terFb.dsTex.bind(gl::TEXTURE2);
        resBank().texH.get("water_norms")->bind(gl::TEXTURE3);
        liquidProg.set_glmmat("reflMat", o->reflMat);
        liquidProg.set_glmvec("flowOffset", o->flowOffs);
        liquidProg.set_glmvec("translation", o->pos + bop.first);
        liquidProg.set_F("scale", o->scale);
        liquidProg.set_F("wSmooth", o->smooth);
        liquidProg.set_F("swing", glm::mix(swingA, swingB, accum / dt));
        gl::BindVertexArray(o->vao);
        gl::DrawArrays(gl::TRIANGLES, 0, 6);
    }


    /// Characters and Player ///

    advmodelProg.use();
    player.model.mesh->bind_buffers(gl::TEXTURE3);
    player.model.bind_textures();
    advmodelProg.set_glmmat("modelMat", player.model.modelMat);
    advmodelProg.set_ptrvec<glm::vec4>("s_quat", player.model.skeleton.quatData);
    advmodelProg.set_ptrvec<glm::vec3>("s_offs", player.model.skeleton.offsData);
    gl::DrawElements(gl::TRIANGLES, player.model.mesh->iCount, gl::UNSIGNED_INT, 0);


    /// Map Models Opaque ///

    modelProg.use();
    for (wld::World::OPair& bop : world.cellObjects) {
        if (bop.second->type == wld::ObjType::Model) {
            wld::Model* o = static_cast<wld::Model*>(bop.second);
            if (o->alpha) continue;

            o->mesh->bind_buffers();
            o->bind_textures();
            modelProg.set_glmmat("modelMat", glm::translate(o->modelMat, bop.first));

            gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
        }
    }


    /// Map Models Translucent ///

    gl::Enable(gl::BLEND);
    for (wld::World::OPair& bop : world.cellObjects) {
        if (bop.second->type == wld::ObjType::Model) {
            wld::Model* o = static_cast<wld::Model*>(bop.second);
            if (!o->alpha) continue;

            o->mesh->bind_buffers();
            o->bind_textures();
            modelProg.set_glmmat("modelMat", glm::translate(o->modelMat, bop.first));

            gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
        }
    }


    /// FXAA and Screen Output ///

    gl::Disable(gl::BLEND);
    gl::Disable(gl::DEPTH_TEST);
    int antialiasing = settings.get_int("antialiasing");
    if (antialiasing) {
        lumaProg.use();
        priFb.cTexVec[0].bind(gl::TEXTURE0);
        screenQuad.draw();

        gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        if (antialiasing == 1) fxaaLProg.use();
        else if (antialiasing == 2) fxaaHProg.use();
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
