#include <random>

#include <sqee/app/application.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/framebuffers.hpp>

#include "settings.hpp"
#include "resbank.hpp"

#include "scenegame.hpp"

inline glm::mat3 to_norm_mat(const glm::mat4& _mvMat) {
    return glm::mat3(glm::transpose(glm::inverse(_mvMat)));
}

using namespace sqt;

SceneGame::SceneGame(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 24;

    world.load_base("WorldTest");

    camera.init({0, 0, 0}, {0.f, 0.475f, -1.f}, {0.5f, 60.f}, {16, 10}, 1.f);
    //camera.init({0, 0, 0}, {0, 0, 0}, 0, 0, 1.2f, 1.f, 20.f);
    camera.update_projMat();
    camera.update_viewMat();
    camera.update_ubo();

    player.test_init();

    resize(app.get_size());
}

void SceneGame::resize(glm::uvec2 _size) {
    camera.aspect = glm::vec2(_size);
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
}

void SceneGame::render(float _ft) {
    static sq::Framebuffer priFb;
    static sq::Framebuffer secFb;
    static sq::Framebuffer terFb;

    static sq::Shader staticVS;
    static sq::Shader skellyVS;
    static sq::Shader modelsFS;
    static sq::Shader staticshadVS;
    static sq::Shader skellyshadVS;
    static sq::Shader liquidVS;
    static sq::Shader liquidFS;
    static sq::Shader quadVS;
    static sq::Shader lumaFS;
    static sq::Shader fxaaLFS;
    static sq::Shader fxaaHFS;
    static sq::Shader passthruFS;

    static sq::Pipeline staticPl;
    static sq::Pipeline skellyPl;
    static sq::Pipeline staticshadPl;
    static sq::Pipeline skellyshadPl;
    static sq::Pipeline liquidPl;
    static sq::Pipeline lumaPl;
    static sq::Pipeline fxaaPl;

    /// Set things up on first call ///

    static bool first = true;
    if (first) { first = false;
        priFb.create({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                     gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
        secFb.create({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                     gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
        terFb.create({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                     gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);

        staticVS.load("models/static_vs", gl::VERTEX_SHADER);
        skellyVS.load("models/skelly_vs", gl::VERTEX_SHADER);
        modelsFS.load("models/models_fs", gl::FRAGMENT_SHADER);
        staticshadVS.load("models/staticshad_vs", gl::VERTEX_SHADER);
        skellyshadVS.load("models/skellyshad_vs", gl::VERTEX_SHADER);
        liquidVS.load("liquids/surface_vs", gl::VERTEX_SHADER);
        liquidFS.load("liquids/surface_fs", gl::FRAGMENT_SHADER);
        quadVS.load("generic/quad_vs", gl::VERTEX_SHADER);
        lumaFS.load("generic/luma_fs", gl::FRAGMENT_SHADER);
        fxaaLFS.load("post/fxaal_fs", gl::FRAGMENT_SHADER);
        fxaaHFS.load("post/fxaah_fs", gl::FRAGMENT_SHADER);
        passthruFS.load("generic/passthru_fs", gl::FRAGMENT_SHADER);

        staticPl.use_shader(staticVS);
        staticPl.use_shader(modelsFS);
        skellyPl.use_shader(skellyVS);
        skellyPl.use_shader(modelsFS);
        staticshadPl.use_shader(staticshadVS);
        skellyshadPl.use_shader(skellyshadVS);
        liquidPl.use_shader(liquidVS);
        liquidPl.use_shader(liquidFS);
        lumaPl.use_shader(quadVS);
        lumaPl.use_shader(lumaFS);
        fxaaPl.use_shader(quadVS);

        staticVS.add_uniform("modelMat"); // mat4
        staticVS.add_uniform("normMat"); // mat3
        staticVS.add_uniform("useRefl"); // int
        skellyVS.add_uniform("modelMat"); // mat4
        skellyVS.add_uniform("normMat"); // mat3
        skellyVS.add_uniform("skelQuat", 40); // vec4
        skellyVS.add_uniform("skelOffs", 40); // vec3
        skellyVS.add_uniform("useRefl"); // int
        modelsFS.add_uniform("shadBiasMod"); // float
        modelsFS.add_uniform("mode"); // int
        staticshadVS.add_uniform("shadMat"); // mat4
        staticshadVS.add_uniform("modelMat"); // mat4
        skellyshadVS.add_uniform("shadMat"); // mat4
        skellyshadVS.add_uniform("modelMat"); // mat4
        skellyshadVS.add_uniform("skelQuat", 40); // vec4
        skellyshadVS.add_uniform("skelOffs", 40); // vec3
        liquidFS.add_uniform("shadBiasMod"); // float
        fxaaLFS.add_uniform("vpPixSize"); // vec2
        fxaaHFS.add_uniform("vpPixSize"); // vec2
    };


    /// Act Upon Changed Settings ///

    if (updateSettings) {
        int shadQuality = vidSet().smInt.crnt("shadQuality");

        float shadBiasMod = std::pow(2.f, 3.f-shadQuality);
        modelsFS.set_sca("shadBiasMod", shadBiasMod);
        liquidFS.set_sca("shadBiasMod", shadBiasMod);

        int fxaaQuality = vidSet().smInt.crnt("fxaaQuality");
        if      (fxaaQuality == 1) fxaaPl.use_shader(fxaaLFS);    // Low
        else if (fxaaQuality == 2) fxaaPl.use_shader(fxaaHFS);    // High
        else                       fxaaPl.use_shader(passthruFS); // Off

        updateSettings = false;
    }


    /// Act Upon Changed Framebuffers ///

    if (updateFramebuffers) {
        glm::uvec2 size = app.get_size();

        priFb.resize(size);
        priFb.cTexVec[0].set_preset(sq::Texture::Preset::L_C);
        secFb.resize(size);
        secFb.cTexVec[0].set_preset(sq::Texture::Preset::L_C);
        terFb.resize(size);
        terFb.cTexVec[0].set_preset(sq::Texture::Preset::L_C);

        glm::vec2 pixSize(1.f / size.x, 1.f / size.y);
        fxaaLFS.set_glmvec("vpPixSize", pixSize);
        fxaaHFS.set_glmvec("vpPixSize", pixSize);

        updateFramebuffers = false;
    }

    gl::DepthFunc(gl::LEQUAL);
    gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
    gl::ClearColor(0.f, 0.f, 0.f, 1.f);

    player.calc(accum, camera);
    world.calc(accum);


    /// Shadow Textures ///

    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    gl::Enable(gl::CULL_FACE);

    world.slFb.use();
    world.slFb.useVP();
    gl::Clear(gl::DEPTH_BUFFER_BIT);
    staticshadVS.set_glmmat("shadMat", world.skylMat);
    skellyshadVS.set_glmmat("shadMat", world.skylMat);

    staticshadPl.bind();
    for (wld::Model* model : world.modelList) {
        if (!model->shad) continue;

        model->mesh->bind_vao();
        staticshadVS.set_glmmat("modelMat", model->modelMat);
        for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
            model->mesh->draw_ibo(i);
        }
    }

    skellyshadPl.bind();
    { // will become loop
        player.model.mesh->bind_vao();
        skellyshadVS.set_glmmat("modelMat", player.model.modelMat);
        skellyshadVS.set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
        skellyshadVS.set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
        for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
            player.model.mesh->draw_ibo(i);
        }
    }

    world.slFb.dsTex.bind(gl::TEXTURE4);

    for (uint i = 0; i < world.spotCount; i++) {
        world.spFbArr[i].use();
        world.spFbArr[i].useVP();
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        staticshadVS.set_glmmat("shadMat", world.lightVec[i]->shadMat);
        skellyshadVS.set_glmmat("shadMat", world.lightVec[i]->shadMat);

        staticshadPl.bind();
        for (wld::Model* model : world.modelList) {
            if (!model->shad) continue;

            model->mesh->bind_vao();
            staticshadVS.set_glmmat("modelMat", model->modelMat);
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                model->mesh->draw_ibo(i);
            }
        }

        skellyshadPl.bind();
        { // will become loop
            player.model.mesh->bind_vao();
            skellyshadVS.set_glmmat("modelMat", player.model.modelMat);
            skellyshadVS.set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
            skellyshadVS.set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                player.model.mesh->draw_ibo(i);
            }
        }
        world.spFbArr[i].dsTex.bind(gl::TEXTURE5+i);
    }


    /// Liquids and submerged areas ///

    priFb.use();
    priFb.useVP();
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

    for (wld::Liquid* liquid : world.liquidList) {
        gl::Enable(gl::CLIP_DISTANCE0);
        gl::CullFace(gl::FRONT);
        liquid->bind_ubo();
        staticVS.set_sca("useRefl", 1);
        skellyVS.set_sca("useRefl", 1);
        secFb.use();
        gl::ClearColor(0.5f, 0.5f, 0.7f, 1.f);
        gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
        gl::ClearColor(0.f, 0.f, 0.f, 1.f);

        //////////////////////////////////////////

        skellyPl.bind();
        { // will become loop
            player.model.mesh->bind_vao();
            skellyVS.set_glmmat("modelMat", player.model.modelMat);
            skellyVS.set_glmmat("normMat", to_norm_mat(camera.viewMat * player.model.modelMat));
            skellyVS.set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
            skellyVS.set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                modelsFS.set_sca("mode", player.model.skin.get_mode(i));
                player.model.skin.bind_textures(i);
                player.model.mesh->draw_ibo(i);
            }
        }

        staticPl.bind();
        for (wld::Model* model : world.modelList) {
            if (!model->refl) continue;
            model->mesh->bind_vao();
            staticVS.set_glmmat("modelMat", model->modelMat);
            staticVS.set_glmmat("normMat", to_norm_mat(camera.viewMat * model->modelMat));
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                modelsFS.set_sca("mode", model->skin.get_mode(i));
                model->skin.bind_textures(i);
                model->mesh->draw_ibo(i);
            }
        }

        ////////////////////////////////////////

        gl::CullFace(gl::BACK);
        staticVS.set_sca("useRefl", 0);
        skellyVS.set_sca("useRefl", 0);
        terFb.use();
        gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

        skellyPl.bind();
        { // will become loop
            player.model.mesh->bind_vao();
            skellyVS.set_glmmat("modelMat", player.model.modelMat);
            skellyVS.set_glmmat("normMat", to_norm_mat(camera.viewMat * player.model.modelMat));
            skellyVS.set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
            skellyVS.set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                modelsFS.set_sca("mode", player.model.skin.get_mode(i));
                player.model.skin.bind_textures(i);
                player.model.mesh->draw_ibo(i);
            }
        }

        staticPl.bind();
        for (wld::Model* model : world.modelList) {
            if (!model->refr) continue;
            model->mesh->bind_vao();
            staticVS.set_glmmat("modelMat", model->modelMat);
            staticVS.set_glmmat("normMat", to_norm_mat(camera.viewMat * model->modelMat));
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                modelsFS.set_sca("mode", model->skin.get_mode(i));
                model->skin.bind_textures(i);
                model->mesh->draw_ibo(i);
            }
        }

        ////////////////////////////////////////

        gl::Disable(gl::CLIP_DISTANCE0);
        priFb.use();

        liquidPl.bind();
        secFb.cTexVec[0].bind(gl::TEXTURE0);
        terFb.cTexVec[0].bind(gl::TEXTURE1);
        terFb.dsTex.bind(gl::TEXTURE2);

        liquid->draw();
    }


    /// Main Rendering

    skellyPl.bind();
    { // will become loop
        player.model.mesh->bind_vao();
        skellyVS.set_glmmat("modelMat", player.model.modelMat);
        skellyVS.set_glmmat("normMat", to_norm_mat(camera.viewMat * player.model.modelMat));
        skellyVS.set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
        skellyVS.set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
        for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
            modelsFS.set_sca("mode", player.model.skin.get_mode(i));
            player.model.skin.bind_textures(i);
            player.model.mesh->draw_ibo(i);
        }
    }

    staticPl.bind();
    for (wld::Model* model : world.modelList) {
        model->mesh->bind_vao();
        staticVS.set_glmmat("modelMat", model->modelMat);
        staticVS.set_glmmat("normMat", to_norm_mat(camera.viewMat * model->modelMat));
        for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
            modelsFS.set_sca("mode", model->skin.get_mode(i));
            model->skin.bind_textures(i);
            model->mesh->draw_ibo(i);
        }
    }


    /// FXAA and/or Screen Output ///

    gl::Disable(gl::DEPTH_TEST);
    priFb.cTexVec[0].bind(gl::TEXTURE0);

    if (vidSet().smInt.crnt("fxaaQuality")) {
        lumaPl.bind();
        sq::draw_screen_quad();
    }
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    gl::Clear(gl::COLOR_BUFFER_BIT);
    fxaaPl.bind();
    sq::draw_screen_quad();

    gl::BindVertexArray(0);
}


bool HandlerGame::handle(const sf::Event&) {
    return false;
}
