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

    level.load_map("new");

    camera.init({0, 0, 0}, 0.375f, 0, 0, 0, 1.2f, 0.5f, 60.f);
    //camera.init({0.f, 0.f, 0.f}, 0.f, 0, 0, 0, 1.17f, 0.5f, 60.f);
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
    static int shadLevel, aaLevel;
    static glm::uvec2 size, margins, adjSize;

    static std::minstd_rand gen(std::time(nullptr)); gen();

    static sq::Framebuffer shadowFb;
    static GLenum noDrawBuffers[] = {gl::NONE};

    static sq::Framebuffer mapFb;
    static sq::Framebuffer blitFb;
    static GLenum drawBuffers[] = {
        gl::COLOR_ATTACHMENT0
    };

    static sq::ScreenQuad screenQuad;

    static sq::Shader modelProg;
    static sq::Shader liquidProg;
    static sq::Shader shadowProg;
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
        mapFb.create(1, drawBuffers, true);
        blitFb.create(1, drawBuffers, false);

        // Shaders
        modelProg.load_from_file("res/shaders/model_vs.glsl", gl::VERTEX_SHADER);
        modelProg.load_from_file("res/shaders/model_fs.glsl", gl::FRAGMENT_SHADER);
        modelProg.build();
        liquidProg.load_from_file("res/shaders/liquid_vs.glsl", gl::VERTEX_SHADER);
        liquidProg.load_from_file("res/shaders/liquid_fs.glsl", gl::FRAGMENT_SHADER);
        liquidProg.build();
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

        modelProg.add_uniform("anim", sq::U_TYPE::u_1i);
        modelProg.add_uniform("percent", sq::U_TYPE::u_1f);
        modelProg.add_uniform("shadQuality", sq::U_TYPE::u_1i);
        modelProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        modelProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        modelProg.add_uniform("modelMat", sq::U_TYPE::u_4m);
        modelProg.add_uniform("shadProjViewMat", sq::U_TYPE::u_4m);
        modelProg.add_uniform("camPos", sq::U_TYPE::u_3f);
        modelProg.add_uniform("skyLightDir", sq::U_TYPE::u_3f);
        modelProg.add_uniform("skyLightDiff", sq::U_TYPE::u_3f);
        modelProg.add_uniform("skyLightAmbi", sq::U_TYPE::u_3f);
        modelProg.add_uniform("skyLightSpec", sq::U_TYPE::u_3f);
        modelProg.add_uniform("texNorm", sq::U_TYPE::u_1i);
        modelProg.add_uniform("texDiff", sq::U_TYPE::u_1i);
        modelProg.add_uniform("texAmbi", sq::U_TYPE::u_1i);
        modelProg.add_uniform("texSpec", sq::U_TYPE::u_1i);
        modelProg.add_uniform("texShad", sq::U_TYPE::u_1i);

        liquidProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        liquidProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        liquidProg.add_uniform("camPos", sq::U_TYPE::u_3f);
        liquidProg.add_uniform("zPos", sq::U_TYPE::u_1f);
        liquidProg.add_uniform("flowOffset", sq::U_TYPE::u_2f);
        liquidProg.add_uniform("scale", sq::U_TYPE::u_1f);
        liquidProg.add_uniform("skyLightDir", sq::U_TYPE::u_3f);
        liquidProg.add_uniform("skyLightSpec", sq::U_TYPE::u_3f);
        liquidProg.add_uniform("swing", sq::U_TYPE::u_1f);
        liquidProg.add_uniform("wSmooth", sq::U_TYPE::u_1f);
        liquidProg.add_uniform("tinge", sq::U_TYPE::u_3f);
        liquidProg.add_uniform("texNorm", sq::U_TYPE::u_1i);
        liquidProg.add_uniform("texEnv", sq::U_TYPE::u_1i);

        shadowProg.add_uniform("anim", sq::U_TYPE::u_1i);
        shadowProg.add_uniform("percent", sq::U_TYPE::u_1f);
        shadowProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        shadowProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        shadowProg.add_uniform("modelMat", sq::U_TYPE::u_4m);

        lumaProg.add_uniform("screenTex", sq::U_TYPE::u_1i);

        fxaaLProg.add_uniform("screenTex", sq::U_TYPE::u_1i);
        fxaaLProg.add_uniform("vpPixSize", sq::U_TYPE::u_2f);
        fxaaHProg.add_uniform("screenTex", sq::U_TYPE::u_1i);
        fxaaHProg.add_uniform("vpPixSize", sq::U_TYPE::u_2f);

        quadProg.add_uniform("screenTex", sq::U_TYPE::u_1i);

        modelProg.use();
        modelProg.set_uniform_i("texNorm", 0);
        modelProg.set_uniform_i("texDiff", 1);
        modelProg.set_uniform_i("texAmbi", 2);
        modelProg.set_uniform_i("texSpec", 3);
        modelProg.set_uniform_i("texShad", 4);

        liquidProg.use();
        liquidProg.set_uniform_i("texNorm", 0);
        liquidProg.set_uniform_i("texEnv", 1);

        lumaProg.use();
        lumaProg.set_uniform_i("screenTex", 0);

        fxaaLProg.use();
        fxaaLProg.set_uniform_i("screenTex", 0);
        fxaaHProg.use();
        fxaaHProg.set_uniform_i("screenTex", 0);

        quadProg.use();
        quadProg.set_uniform_i("screenTex", 0);

        sq::Texture::Ptr tex = sq::tex2DArray_load_blank({1024, 1024, 2}, gl::RGB8);
        sq::tex2DArray_add_file(tex, "res/textures/static/water_norm1.png", 0);
        sq::tex2DArray_add_file(tex, "res/textures/static/water_norm2.png", 1);
        tex->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        tex->set_params(2, sq::S_T_WRAP, sq::BOTH_REPEAT);
        texHolder.set("waterNorm", tex);

        first = false;
    }

    if (modSettings) {
        if (settings.shad) {
            shadowFb.create(0, noDrawBuffers, true);
            if (settings.shad == 1)
                shadowFb.resize({1024, 1024}, {0, 0}); // Low
            else if (settings.shad == 2)
                shadowFb.resize({2048, 2048}, {0, 0}); // Medium
            else if (settings.shad == 3)
                shadowFb.resize({4096, 4096}, {0, 0}); // High
            else if (settings.shad == 4)
                shadowFb.resize({8192, 8192}, {0, 0}); // Extreme
            shadowFb.get(-1)->bind();
            shadowFb.get(-1)->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
            shadowFb.get(-1)->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        }
        modelProg.use();
        modelProg.set_uniform_i("shadQuality", settings.shad);

        aaLevel = settings.aa;
        shadLevel = settings.shad;

        modSettings = false;
    }

    if (updateFramebuffers) {
        size = app->get_size();
        margins = {size.x / 10, size.y / 10};
        adjSize = size + margins + margins;
        gl::Viewport(0, 0, size.x, size.y);

        // Main Framebuffer
        mapFb.resize(size, margins);
        mapFb.get(0)->bind();
        mapFb.get(0)->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);

        blitFb.resize(size, margins);
        blitFb.get(0)->bind();
        blitFb.get(0)->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);

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

        liquidProg.use();
        liquidProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        liquidProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));

        shadowProg.use();
        shadowProg.set_uniform_mv("projMat", false, glm::value_ptr(skyLight.projMat));
        shadowProg.set_uniform_mv("viewMat", false, glm::value_ptr(skyLight.viewMat));

        updateSkyLight = false;
    }

    player.update_render(dt, accum);
    level.update_render(_ft);
    modelProg.use();
    modelProg.set_uniform_mv("projMat", false, glm::value_ptr(camera.projMat));
    modelProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));
    modelProg.set_uniform_fv("camPos", glm::value_ptr(camera.pos));

    liquidProg.use();
    liquidProg.set_uniform_mv("projMat", false, glm::value_ptr(camera.projMat));
    liquidProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));
    liquidProg.set_uniform_fv("camPos", glm::value_ptr(camera.pos));


    /// Shadow Texture ///

    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    if (shadLevel) {
        shadowFb.use();
        shadowFb.useVP(false);
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        shadowProg.use();

        for (auto& l : level.objectMapVec) {
            for (auto& bop : l) {
                if (bop.second->type == obj::Type::Model) {
                   obj::Model::Ptr o = std::static_pointer_cast<obj::Model>(bop.second);
                    if (!o->shad) continue;
                    shadowProg.set_uniform_i("anim", o->mesh->anim);
                    if (o->mesh->anim)
                        shadowProg.set_uniform_f("percent", o->mesh->accum / (dt * o->mesh->frac));
                    shadowProg.set_uniform_mv("modelMat", false, glm::value_ptr(o->modelMat));

                    gl::BindVertexArray(o->mesh->vao);
                    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, o->mesh->ibo);
                    gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
                }
            }
        }

        /////////////////////////////////
        /// PLAYER MODEL TODO         ///
        /////////////////////////////////
    }


    mapFb.use();
    mapFb.useVP(false);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
    if (shadLevel) shadowFb.get(-1)->bind(gl::TEXTURE4);

    for (std::map<std::string, obj::Ptr>& layer : level.objectMapVec) {

    /// Map ///

    /// Map Models Opaque ///
    modelProg.use();
    for (std::pair<const std::string, obj::Ptr>& bo : layer) {
        if (bo.second->type == obj::Type::Model) {
            obj::Model::Ptr o = std::static_pointer_cast<obj::Model>(bo.second);
            if (o->skin->alpha) continue;
            o->skin->texNorm->bind(gl::TEXTURE0);
            o->skin->texDiff->bind(gl::TEXTURE1);
            o->texAmbi->bind(gl::TEXTURE2);
            o->skin->texSpec->bind(gl::TEXTURE3);
            modelProg.set_uniform_i("anim", o->mesh->anim);
            if (o->mesh->anim)
                modelProg.set_uniform_f("percent", o->mesh->accum / (dt * o->mesh->frac));
            modelProg.set_uniform_mv("modelMat", false, glm::value_ptr(o->modelMat));

            gl::BindVertexArray(o->mesh->vao);
            gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, o->mesh->ibo);
            gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
        }
    }

    /// Characters and Player ///
    /////////////////////////////////
    /// PLAYER MODEL TODO         ///
    /////////////////////////////////


    /// Liquids ///
    bool setupLiquid = false;
    for (std::pair<const std::string, obj::Ptr>& bo : layer) {
        if (bo.second->type == obj::Type::Liquid) {
            if (!setupLiquid) {
                gl::BindFramebuffer(gl::READ_FRAMEBUFFER, mapFb.framebuffer);
                gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, blitFb.framebuffer);
                gl::BlitFramebuffer(0, 0, adjSize.x, adjSize.y, 0, 0, adjSize.x, adjSize.y,
                                    gl::COLOR_BUFFER_BIT, gl::NEAREST);
                mapFb.use();
                liquidProg.use();
                texHolder.get("waterNorm")->bind(gl::TEXTURE0);
                blitFb.get(0)->bind(gl::TEXTURE1);
                setupLiquid = true;
            }
            obj::Liquid::Ptr o = std::static_pointer_cast<obj::Liquid>(bo.second);
            glm::vec2 offset = o->flowOffsetA * float(((dt - accum) / dt)) +
                               o->flowOffsetB * float((accum / dt));
            liquidProg.set_uniform_f("zPos", o->zPos);
            liquidProg.set_uniform_fv("flowOffset", glm::value_ptr(offset));
            liquidProg.set_uniform_f("scale", o->scale);
            liquidProg.set_uniform_f("wSmooth", o->wSmooth);
            liquidProg.set_uniform_f("swing", swingA * ((dt - accum) / dt) + swingB * (accum / dt));
            liquidProg.set_uniform_fv("tinge", glm::value_ptr(o->tinge));
            gl::BindVertexArray(o->vao);
            gl::DrawArrays(gl::TRIANGLES, 0, 6);
        }
    }


    /// Map Models Translucent ///
    gl::Enable(gl::BLEND);
    modelProg.use();
    for (std::pair<const std::string, obj::Ptr>& bo : layer) {
        if (bo.second->type == obj::Type::Model) {
            obj::Model::Ptr o = std::static_pointer_cast<obj::Model>(bo.second);
            if (!o->skin->alpha) continue;
            o->skin->texNorm->bind(gl::TEXTURE0);
            o->skin->texDiff->bind(gl::TEXTURE1);
            o->texAmbi->bind(gl::TEXTURE2);
                o->skin->texSpec->bind(gl::TEXTURE3);
            modelProg.set_uniform_i("anim", o->mesh->anim);
            if (o->mesh->anim)
                modelProg.set_uniform_f("percent", o->mesh->accum / (dt * o->mesh->frac));
            modelProg.set_uniform_mv("modelMat", false, glm::value_ptr(o->modelMat));

            gl::BindVertexArray(o->mesh->vao);
            gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, o->mesh->ibo);
            gl::DrawElements(gl::TRIANGLES, o->mesh->iCount, gl::UNSIGNED_INT, 0);
        }
    }
    gl::Disable(gl::BLEND);

    }

    /// FXAA ///
    gl::Disable(gl::DEPTH_TEST);
    if (aaLevel) { // Do FXAA
        /// Put Luma into Alpha ///
        lumaProg.use();
        mapFb.get(0)->bind(gl::TEXTURE0);
        screenQuad.draw();

        /// FXAA and screen output ///
        gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        mapFb.useVP(true);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        if (aaLevel == 1) fxaaLProg.use();
        else if (aaLevel == 2) fxaaHProg.use();
        screenQuad.draw();
    } else { // No FXAA
        gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        mapFb.useVP(true);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        quadProg.use();
        mapFb.get(0)->bind(gl::TEXTURE0);
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
