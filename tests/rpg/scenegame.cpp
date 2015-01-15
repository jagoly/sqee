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

SceneGame::SceneGame(sq::Application& _app)
    : sq::Scene(_app), camera(true) {

    tickRate = 24;

    world.load_base("WorldTest");

    camera.pos = {0, 0, 0};
    camera.dir = {0.f, 0.475f, -1.f};
    camera.range = {0.5f, 60.f};
    camera.size = {16, 10};
    camera.fov = 1.f;
    camera.strictAspect = 1.6f;
    camera.update();

    player.test_init();
    player.camera = &camera;
    player.world = &world;

    resize(app.get_size());
}

void SceneGame::resize(glm::uvec2 _size) {
    camera.size = glm::vec2(_size);
    updateFramebuffers = true;
}

void SceneGame::update() {
    world.tick();

    static list<sq::Direction> keys;
    static map<sq::Direction, bool> keyStates = {
        {sq::Direction::North, false}, {sq::Direction::East, false},
        {sq::Direction::South, false}, {sq::Direction::West, false}
    };
    map<sq::Direction, bool> keyStatesPrev = keyStates;

    keyStates[sq::Direction::North] = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    keyStates[sq::Direction::East]  = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    keyStates[sq::Direction::South] = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    keyStates[sq::Direction::West]  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);

    for (const pair<sq::Direction, bool>& dpPair : keyStates) {
        if (dpPair.second && !keyStatesPrev.at(dpPair.first)) {
            keys.remove(dpPair.first); keys.emplace_front(dpPair.first);
        } else
        if (!dpPair.second && keyStatesPrev.at(dpPair.first)) {
            keys.remove(dpPair.first);
        }
    }

    player.attempt_move(keys.empty() ? sq::Direction::Zero : keys.front());
    player.tick();
}

void SceneGame::render(float _ft) {
    static sq::Framebuffer priFb({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                                 gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
    static sq::Framebuffer secFb({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                                 gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
    static sq::Framebuffer terFb({gl::COLOR_ATTACHMENT0}, {gl::RGBA}, {gl::RGBA8},
                                 gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
    static sq::Framebuffer slFramebuf(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
    static sq::Framebuffer spFramebufs[8] {
        {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32}, {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32},
        {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32}, {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32},
        {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32}, {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32},
        {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32}, {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32}
    };

    static sq::Shader staticshadVS;
    static sq::Shader skellyshadVS;
    static sq::Shader punchFS;
    static sq::Shader staticVS;
    static sq::Shader skellyVS;
    static sq::Shader modelsFS;
    static sq::Shader liquidVS;
    static sq::Shader liquidFS;
    static sq::Shader quadVS;
    static sq::Shader lumaFS;
    static sq::Shader fxaaLFS;
    static sq::Shader fxaaHFS;
    static sq::Shader passthruFS;

    static sq::Pipeline pipeLine;


    /// Set things up on first call ///

    static bool first = true;
    if (first) { first = false;
        staticshadVS.load("models/staticshad_vs", gl::VERTEX_SHADER);
        skellyshadVS.load("models/skellyshad_vs", gl::VERTEX_SHADER);
        punchFS.load("models/punch_fs", gl::FRAGMENT_SHADER);
        staticVS.load("models/static_vs", gl::VERTEX_SHADER);
        skellyVS.load("models/skelly_vs", gl::VERTEX_SHADER);
        modelsFS.load("models/models_fs", gl::FRAGMENT_SHADER);
        liquidVS.load("liquids/surface_vs", gl::VERTEX_SHADER);
        liquidFS.load("liquids/surface_fs", gl::FRAGMENT_SHADER);
        quadVS.load("generic/quad_vs", gl::VERTEX_SHADER);
        lumaFS.load("generic/luma_fs", gl::FRAGMENT_SHADER);
        fxaaLFS.load("post/fxaal_fs", gl::FRAGMENT_SHADER);
        fxaaHFS.load("post/fxaah_fs", gl::FRAGMENT_SHADER);
        passthruFS.load("generic/passthru_fs", gl::FRAGMENT_SHADER);

        staticshadVS.add_uniform("shadMat"); // mat4
        staticshadVS.add_uniform("modelMat"); // mat4
        skellyshadVS.add_uniform("shadMat"); // mat4
        skellyshadVS.add_uniform("modelMat"); // mat4
        skellyshadVS.add_uniform("skelQuat", 40); // vec4
        skellyshadVS.add_uniform("skelOffs", 40); // vec3
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
        liquidFS.add_uniform("shadBiasMod"); // float
        fxaaLFS.add_uniform("vpPixSize"); // vec2
        fxaaHFS.add_uniform("vpPixSize"); // vec2
    };


    /// Run calc() of other objects ///

    player.calc(accum);
    world.calc(accum);


    /// Act Upon Changed Settings ///

    if (vidSet().check_update("SceneGame")) {
        int shadQuality = vidSet().smInt.crnt("shadQuality");

        float shadBiasMod = std::pow(2.f, 2.f-shadQuality);
        modelsFS.set_sca("shadBiasMod", shadBiasMod);
        liquidFS.set_sca("shadBiasMod", shadBiasMod);
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


    /// World Updates ///

    if (world.updateScene) { world.updateScene = false;
        uint shadowMult = std::pow(2, vidSet().smInt.crnt("shadQuality"));
        slFramebuf.resize(world.skylTexSize * shadowMult);
        slFramebuf.dsTex.set_preset(sq::Texture::Preset::SHAD);

        for (uint i = 0; i < world.spotCount; i++) {
            spFramebufs[i].resize(world.lightVec[i]->texSize * shadowMult);
            spFramebufs[i].dsTex.set_preset(sq::Texture::Preset::SHAD);
        }
    }


    /// Per-frame setup ///

    gl::DepthFunc(gl::LEQUAL);
    gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
    gl::ClearColor(0.f, 0.f, 0.f, 1.f);
    pipeLine.bind();


    /// Skylight ///

    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    gl::Enable(gl::CULL_FACE);

    if (world.skylEnable) {
        pipeLine.disable_stages(gl::FRAGMENT_SHADER_BIT);

        slFramebuf.use();
        slFramebuf.useVP();
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        staticshadVS.set_glmmat("shadMat", world.skylMat);
        skellyshadVS.set_glmmat("shadMat", world.skylMat);

        pipeLine.use_shader(skellyshadVS);
        { // will become loop
            skellyshadVS.set_glmmat("modelMat", player.model.modelMat);
            skellyshadVS.set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
            skellyshadVS.set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
            player.model.mesh->bind_vao();
            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                player.model.mesh->draw_ibo(i);
            }
        }

        pipeLine.use_shader(staticshadVS);
        for (Model* model : world.modelList) {
            if (!model->shad || model->pnch) continue;
            staticshadVS.set_glmmat("modelMat", model->modelMat);
            model->mesh->bind_vao();
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                model->mesh->draw_ibo(i);
            }
        }

        pipeLine.use_shader(punchFS);
        for (Model* model : world.modelList) {
            if (!model->shad || !model->pnch) continue;
            staticshadVS.set_glmmat("modelMat", model->modelMat);
            model->mesh->bind_vao();
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                model->skin.bind_textures(i, sq::Skin::Mode::diff);
                model->mesh->draw_ibo(i);
            }
        }

        slFramebuf.dsTex.bind(gl::TEXTURE4);
    }


    /// Spotlights ///

    for (uint i = 0; i < world.spotCount; i++) {
        spFramebufs[i].use();
        spFramebufs[i].useVP();
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        pipeLine.disable_stages(gl::FRAGMENT_SHADER_BIT);
        staticshadVS.set_glmmat("shadMat", world.lightVec[i]->shadMat);
        skellyshadVS.set_glmmat("shadMat", world.lightVec[i]->shadMat);

        pipeLine.use_shader(skellyshadVS);
        { // will become loop
            skellyshadVS.set_glmmat("modelMat", player.model.modelMat);
            skellyshadVS.set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
            skellyshadVS.set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
            player.model.mesh->bind_vao();
            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                player.model.mesh->draw_ibo(i);
            }
        }

        pipeLine.use_shader(staticshadVS);
        for (Model* model : world.modelList) {
            if (!model->shad || model->pnch) continue;
            staticshadVS.set_glmmat("modelMat", model->modelMat);
            model->mesh->bind_vao();
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                model->mesh->draw_ibo(i);
            }
        }

        pipeLine.use_shader(punchFS);
        for (Model* model : world.modelList) {
            if (!model->shad || !model->pnch) continue;
            staticshadVS.set_glmmat("modelMat", model->modelMat);
            model->mesh->bind_vao();
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                model->skin.bind_textures(i, sq::Skin::Mode::diff);
                model->mesh->draw_ibo(i);
            }
        }

        spFramebufs[i].dsTex.bind(gl::TEXTURE5+i);
    }


    /// Liquids and submerged areas ///

    priFb.use();
    priFb.useVP();
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

    for (Liquid* liquid : world.liquidList) {
        gl::Enable(gl::CLIP_DISTANCE0);
        gl::CullFace(gl::FRONT);
        liquid->bind_ubo();
        staticVS.set_sca("useRefl", 1);
        skellyVS.set_sca("useRefl", 1);
        secFb.use();
        gl::ClearColor(0.5f, 0.5f, 0.7f, 1.f);
        gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
        gl::ClearColor(0.f, 0.f, 0.f, 1.f);

        pipeLine.use_shader(modelsFS);

        //////////////////////////////////////////

        pipeLine.use_shader(skellyVS);
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

        pipeLine.use_shader(staticVS);
        for (Model* model : world.modelList) {
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

        pipeLine.use_shader(skellyVS);
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

        pipeLine.use_shader(staticVS);
        for (Model* model : world.modelList) {
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

        pipeLine.use_shader(liquidVS);
        pipeLine.use_shader(liquidFS);
        secFb.cTexVec[0].bind(gl::TEXTURE0);
        terFb.cTexVec[0].bind(gl::TEXTURE1);
        terFb.dsTex.bind(gl::TEXTURE2);

        liquid->draw();
    }


    /// Main Rendering ///

    pipeLine.use_shader(modelsFS);

    pipeLine.use_shader(skellyVS);
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

    pipeLine.use_shader(staticVS);
    for (Model* model : world.modelList) {
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

    pipeLine.use_shader(quadVS);

    gl::Disable(gl::DEPTH_TEST);
    priFb.cTexVec[0].bind(gl::TEXTURE0);
    int fxaaQuality = vidSet().smInt.crnt("fxaaQuality");
    if (fxaaQuality > 0) {
        pipeLine.use_shader(lumaFS);
        sq::draw_screen_quad();
        if (fxaaQuality == 1) pipeLine.use_shader(fxaaLFS);
        else pipeLine.use_shader(fxaaHFS);
    } else pipeLine.use_shader(passthruFS);

    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    gl::Clear(gl::COLOR_BUFFER_BIT);
    sq::draw_screen_quad();

    gl::BindVertexArray(0);
}

bool HandlerGame::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::S) {
            int crnt = vidSet().smInt.crnt("shadQuality");
            if (crnt == 2) vidSet().smInt.modify("shadQuality", 0);
            else vidSet().smInt.modify("shadQuality", crnt+1);
            vidSet().apply();
            return true;
        }
        if (_event.key.code == sf::Keyboard::A) {
            int crnt = vidSet().smInt.crnt("fxaaQuality");
            if (crnt == 2) vidSet().smInt.modify("fxaaQuality", 0);
            else vidSet().smInt.modify("fxaaQuality", crnt+1);
            vidSet().apply();
            return true;
        }
    }
    return false;
}
