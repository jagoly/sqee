#include <list>
#include <map>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#ifdef SQEE_DEBUG
#include <sqee/debug/miscdraw.hpp>
#endif

#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/gl/misc.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/models/mesh.hpp>
#include <sqee/models/skin.hpp>
#include <sqee/app/application.hpp>

#include "scenegame.hpp"

inline glm::mat3 to_norm_mat(const glm::mat4& _mvMat) {
    return glm::mat3(glm::transpose(glm::inverse(_mvMat)));
}

using namespace sqt;

SceneGame::SceneGame(sq::Application& _app) : sq::Scene(_app), camera(true), world(_app) {
    tickRate = 24;

    world.load_base("WorldTest");

    camera.pos = {0, 0, 0};
    camera.dir = {0.f, 1.f, 0.f};
    camera.range = {0.1f, 64.f};
    camera.size = {16, 10};
    camera.fov = 1.f;
    camera.aspect = 1.6f;
    camera.update();

    player.test_init();
    player.camera = &camera;
    player.world = &world;

    resize(app.get_size());

    /// Add Settings
    app.settings.add<float>("viewDistance", 64.f);
    app.settings.add<int>("shadQuality", 2);
    app.settings.add<int>("csmCntSkyl", 6);
    app.settings.add<int>("csmCntSpot", 4);
    app.settings.add<int>("fxaaQuality", 2);

    /// Create Framebuffers
    FB.pri.reset(new sq::Framebuffer());
    FB.sec.reset(new sq::Framebuffer());
    FB.ter.reset(new sq::Framebuffer());
    for (auto& fb : FB.skyl)
        fb.reset(new sq::Framebuffer());
    for (auto& arr : FB.spotArr)
        for (auto& fb : arr)
            fb.reset(new sq::Framebuffer());

    /// Create Textures
    TX.cPri.reset(new sq::Texture());
    TX.dPri.reset(new sq::Texture());
    TX.cSec.reset(new sq::Texture());
    TX.dSec.reset(new sq::Texture());
    TX.cTer.reset(new sq::Texture());
    TX.dTer.reset(new sq::Texture());
    TX.dSkyl.reset(new sq::Texture());
    for (auto& tx : TX.dSpotArr)
        tx.reset(new sq::Texture());

    /// Setup Textures
    using TexPreset = sq::Texture::Preset;
    TX.cPri->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, TexPreset::L_C);
    TX.dPri->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32, TexPreset::L_C);
    TX.cSec->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, TexPreset::L_C);
    TX.dSec->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32, TexPreset::L_C);
    TX.cTer->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, TexPreset::L_C);
    TX.dTer->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32, TexPreset::L_C);
    TX.dSkyl->create(gl::TEXTURE_2D_ARRAY, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, TexPreset::L_C);
    TX.dSkyl->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    for (auto& tx : TX.dSpotArr) {
        tx->create(gl::TEXTURE_2D_ARRAY, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, TexPreset::L_C);
        tx->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    }

    /// Setup Framebuffers
    FB.pri->bind();
    FB.pri->attach_2d(gl::COLOR_ATTACHMENT0, *TX.cPri);
    FB.pri->attach_2d(gl::DEPTH_ATTACHMENT, *TX.dPri);
    FB.sec->bind();
    FB.sec->attach_2d(gl::COLOR_ATTACHMENT0, *TX.cSec);
    FB.sec->attach_2d(gl::DEPTH_ATTACHMENT, *TX.dSec);
    FB.ter->bind();
    FB.ter->attach_2d(gl::COLOR_ATTACHMENT0, *TX.cTer);
    FB.ter->attach_2d(gl::DEPTH_ATTACHMENT, *TX.dTer);
    for (int i = 0; i < 6; i++) {
        FB.skyl[i]->bind();
        FB.skyl[i]->attach_layer(gl::DEPTH_ATTACHMENT, *TX.dSkyl, i);
    }
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 4; i++) {
            FB.spotArr[j][i]->bind();
            FB.spotArr[j][i]->attach_layer(gl::DEPTH_ATTACHMENT, *TX.dSpotArr[j], i);
        }
    }

    /// Import GLSL Headers
    app.preproc.import_header("headers/camera_block");
    app.preproc.import_header("headers/world_block");
    app.preproc.import_header("headers/liquid_block");
    app.preproc.import_header("headers/sample_shadow");

    /// Create Shaders
    VS.shad_static.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_static_punch.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_skelly_punch.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.shad_punch.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.modl_static.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.modl_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.modl_frag.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.liqd_surface.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.liqd_surface.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.gnrc_quad.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.gnrc_luma.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_passthru.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_passthru_layer.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.post_fxaa_low.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.post_fxaa_high.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.dbug_lines.reset(new sq::Shader(gl::VERTEX_SHADER));

    /// Load Shaders
    VS.shad_static->load(app.preproc("shadows/static_vs"));
    VS.shad_skelly->load(app.preproc("shadows/skelly_vs"));
    VS.shad_static_punch->load(app.preproc("shadows/static_vs", "#define PUNCH"));
    VS.shad_skelly_punch->load(app.preproc("shadows/skelly_vs", "#define PUNCH"));
    FS.shad_punch->load(app.preproc("shadows/punch_fs"));
    VS.modl_static->load(app.preproc("models/static_vs"));
    VS.modl_skelly->load(app.preproc("models/skelly_vs"));
    FS.modl_frag->load(app.preproc("models/frag_fs"));
    VS.liqd_surface->load(app.preproc("liquids/surface_vs"));
    FS.liqd_surface->load(app.preproc("liquids/surface_fs"));
    VS.gnrc_quad->load(app.preproc("generic/quad_vs"));
    FS.gnrc_luma->load(app.preproc("generic/luma_fs"));
    FS.gnrc_passthru->load(app.preproc("generic/passthru_fs"));
    FS.gnrc_passthru_layer->load(app.preproc("generic/passthru_fs", "#define LAYER"));
    FS.post_fxaa_low->load(app.preproc("postprocess/fxaa_fs", "#define FXAA_QUALITY__PRESET 10"));
    FS.post_fxaa_high->load(app.preproc("postprocess/fxaa_fs", "#define FXAA_QUALITY__PRESET 39"));
    VS.dbug_lines->load(app.preproc("debug/lines_vs"));

    /// Add Uniforms to Shaders
    VS.shad_static->add_uniform("shadMat"); // mat4
    VS.shad_static->add_uniform("modelMat"); // mat4
    VS.shad_skelly->add_uniform("shadMat"); // mat4
    VS.shad_skelly->add_uniform("modelMat"); // mat4
    VS.shad_skelly->add_uniform("skelQuat", 40); // vec4
    VS.shad_skelly->add_uniform("skelOffs", 40); // vec3
    VS.shad_static_punch->add_uniform("shadMat"); // mat4
    VS.shad_static_punch->add_uniform("modelMat"); // mat4
    VS.shad_skelly_punch->add_uniform("shadMat"); // mat4
    VS.shad_skelly_punch->add_uniform("modelMat"); // mat4
    VS.shad_skelly_punch->add_uniform("skelQuat", 40); // vec4
    VS.shad_skelly_punch->add_uniform("skelOffs", 40); // vec3
    VS.modl_static->add_uniform("modelMat"); // mat4
    VS.modl_static->add_uniform("normMat"); // mat3
    VS.modl_static->add_uniform("useRefl"); // int
    VS.modl_skelly->add_uniform("modelMat"); // mat4
    VS.modl_skelly->add_uniform("normMat"); // mat3
    VS.modl_skelly->add_uniform("skelQuat", 40); // vec4
    VS.modl_skelly->add_uniform("skelOffs", 40); // vec3
    VS.modl_skelly->add_uniform("useRefl"); // int
    FS.modl_frag->add_uniform("mode"); // int
    FS.gnrc_passthru_layer->add_uniform("layer"); // int
    FS.post_fxaa_low->add_uniform("vpPixSize"); // vec2
    FS.post_fxaa_high->add_uniform("vpPixSize"); // vec2
    VS.dbug_lines->add_uniform("projViewMat"); // mat4

    camera.recalc_frustums();
}

void SceneGame::resize(glm::uvec2 _size) {
    camera.size = glm::vec2(_size);
    updateFramebuffers = true;
}

void SceneGame::update() {
    world.tick();

    static std::list<sq::Direction> keys;
    static std::map<sq::Direction, bool> keyStates = {
        {sq::Direction::North, false}, {sq::Direction::East, false},
        {sq::Direction::South, false}, {sq::Direction::West, false}
    };
    std::map<sq::Direction, bool> keyStatesPrev = keyStates;

    keyStates[sq::Direction::North] = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    keyStates[sq::Direction::East]  = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    keyStates[sq::Direction::South] = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    keyStates[sq::Direction::West]  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);

    for (const auto& dpPair : keyStates) {
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
    static sq::Pipeline pipeLine;


    static bool first = true;
    if (first) { first = false;
    }


    /// Run calc() of other objects ///

    player.calc(accum);
    world.calc(accum);


    /// Act Upon Changed Settings ///

    if (app.settings.check_update("SceneGame")) {
        int cascadeCntA = app.settings.crnt<int>("csmCntSkyl");
        camera.csmVecA.resize(cascadeCntA);

        float viewDistance = app.settings.crnt<float>("viewDistance");
        camera.range.y = viewDistance;

        uint texSize = 1024.f;
        if (app.settings.crnt<int>("shadQuality") == 1) texSize = 2048.f;
        else if (app.settings.crnt<int>("shadQuality") == 2) texSize = 4096.f;
        TX.dSkyl->resize({texSize, texSize, camera.csmVecA.size()});
    }


    /// Act Upon Changed Framebuffers ///

    if (updateFramebuffers) {
        auto size = app.get_size();
        TX.cPri->resize({size.x, size.y, 1});
        TX.dPri->resize({size.x, size.y, 1});
        TX.cSec->resize({size.x, size.y, 1});
        TX.dSec->resize({size.x, size.y, 1});
        TX.cTer->resize({size.x, size.y, 1});
        TX.dTer->resize({size.x, size.y, 1});

        glm::vec2 pixSize(1.f / size.x, 1.f / size.y);
        FS.post_fxaa_low->set_glmvec("vpPixSize", pixSize);
        FS.post_fxaa_high->set_glmvec("vpPixSize", pixSize);

        updateFramebuffers = false;
    }


    /// World Updates ///

    if (world.updateScene) { world.updateScene = false;
        for (uint i = 0; i < world.spotCount; i++) {
            //uint sz = world.lightVec[i]->texSize * shadMult;
            //spotFbArr[i].resize({sz, sz});
        }
    }


    /// Per-frame setup ///

    gl::DepthFunc(gl::LEQUAL);
    gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
    pipeLine.bind();


    /// Skylight ///

    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    gl::Disable(gl::CULL_FACE);

    if (world.skylEnable) {
        gl::Enable(gl::DEPTH_CLAMP);
        gl::CullFace(gl::FRONT);

        gl::Viewport(0, 0, TX.dSkyl->size.x, TX.dSkyl->size.y);


        //for (uint csm = 0; csm < camera.frusVecA.size(); csm++) {
        for (uint csm = 0; csm < world.skylMatVec.size(); csm++) {
            FB.skyl[csm]->use();
            gl::Clear(gl::DEPTH_BUFFER_BIT);
            VS.shad_static->set_glmmat("shadMat", world.skylMatVec[csm]);
            VS.shad_skelly->set_glmmat("shadMat", world.skylMatVec[csm]);
            VS.shad_static_punch->set_glmmat("shadMat", world.skylMatVec[csm]);
            VS.shad_skelly_punch->set_glmmat("shadMat", world.skylMatVec[csm]);

            pipeLine.disable_stages(gl::FRAGMENT_SHADER_BIT);

            // TODO: fix when skel models support punchthrough alpha
            pipeLine.use_shader(*VS.shad_skelly);
            { // will become loop
                VS.shad_skelly->set_glmmat("modelMat", player.model.modelMat);
                VS.shad_skelly->set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
                VS.shad_skelly->set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
                player.model.mesh->bind_vao();
                for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                    player.model.mesh->draw_ibo(i);
                }
            }

            pipeLine.use_shader(*VS.shad_static);
            for (auto& model : world.filtered<Model>([](const Model& m)
                    { return m.shad && !m.pnch; })) {
                VS.shad_static->set_glmmat("modelMat", model->modelMat);
                model->mesh->bind_vao();
                for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                    model->mesh->draw_ibo(i);
                }
            }

            pipeLine.use_shader(*VS.shad_static_punch);
            pipeLine.use_shader(*FS.shad_punch);
            for (auto& model : world.filtered<Model>([](const Model& m)
                    { return m.shad && m.pnch; })) {
                VS.shad_static_punch->set_glmmat("modelMat", model->modelMat);
                model->mesh->bind_vao();
                for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                    model->skin->bind_textures(i, sq::Skin::Mode::diff);
                    model->mesh->draw_ibo(i);
                }
            }
        }
        TX.dSkyl->bind(gl::TEXTURE4);
        gl::Disable(gl::DEPTH_CLAMP);
    }
    gl::Enable(gl::CULL_FACE);

    /// Spotlights ///

//    for (uint i = 0; i < world.spotCount; i++) {
//        spotFbArr[i].use();
//        spotFbArr[i].useVP();
//        gl::Clear(gl::DEPTH_BUFFER_BIT);
//        pipeLine.disable_stages(gl::FRAGMENT_SHADER_BIT);
//        staticshadVS.set_glmmat("shadMat", world.lightVec[i]->shadMat);
//        skellyshadVS.set_glmmat("shadMat", world.lightVec[i]->shadMat);

//        pipeLine.use_shader(skellyshadVS);
//        { // will become loop
//            skellyshadVS.set_glmmat("modelMat", player.model.modelMat);
//            skellyshadVS.set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
//            skellyshadVS.set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
//            player.model.mesh->bind_vao();
//            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
//                player.model.mesh->draw_ibo(i);
//            }
//        }

//        pipeLine.use_shader(staticshadVS);
//        for (Model* model : world.modelList) {
//            if (!model->shad || model->pnch) continue;
//            staticshadVS.set_glmmat("modelMat", model->modelMat);
//            model->mesh->bind_vao();
//            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
//                model->mesh->draw_ibo(i);
//            }
//        }

//        pipeLine.use_shader(punchFS);
//        for (Model* model : world.modelList) {
//            if (!model->shad || !model->pnch) continue;
//            staticshadVS.set_glmmat("modelMat", model->modelMat);
//            model->mesh->bind_vao();
//            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
//                model->skin->bind_textures(i, sq::Skin::Mode::diff);
//                model->mesh->draw_ibo(i);
//            }
//        }

//        spotFbArr[i].dsTex->bind(gl::TEXTURE8+i);
//    }


    /// Liquids and submerged areas ///

    FB.pri->use();
    gl::Viewport(0, 0, app.get_size().x, app.get_size().y);

    // TEMP
    gl::ClearColor(0.f, 0.1f, 0.2f, 1.f);

    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

    for (auto& liquid : world.filtered<Liquid>()) {
        gl::Enable(gl::CLIP_DISTANCE0);
        liquid->bind_ubo();
        VS.modl_static->set_sca("useRefl", 1);
        VS.modl_skelly->set_sca("useRefl", 1);
        FB.sec->use();
        gl::ClearColor(0.5f, 0.5f, 0.7f, 1.f);
        gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
        gl::ClearColor(0.f, 0.f, 0.f, 1.f);

        pipeLine.use_shader(*FS.modl_frag);

        //////////////////////////////////////////

        pipeLine.use_shader(*VS.modl_skelly);
        { // will become loop
            player.model.mesh->bind_vao();
            VS.modl_skelly->set_glmmat("modelMat", player.model.modelMat);
            VS.modl_skelly->set_glmmat("normMat", to_norm_mat(camera.viewMat * player.model.modelMat));
            VS.modl_skelly->set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
            VS.modl_skelly->set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                FS.modl_frag->set_sca("mode", player.model.skin->get_mode(i));
                player.model.skin->bind_textures(i);
                player.model.mesh->draw_ibo(i);
            }
        }

        pipeLine.use_shader(*VS.modl_static);
        for (auto& model : world.filtered<Model>([](const Model& m)
                { return m.refl; })) {
            model->mesh->bind_vao();
            VS.modl_static->set_glmmat("modelMat", model->modelMat);
            VS.modl_static->set_glmmat("normMat", to_norm_mat(camera.viewMat * model->modelMat));
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                FS.modl_frag->set_sca("mode", model->skin->get_mode(i));
                model->skin->bind_textures(i);
                model->mesh->draw_ibo(i);
            }
        }

        ////////////////////////////////////////

        gl::CullFace(gl::BACK);
        VS.modl_static->set_sca("useRefl", 0);
        VS.modl_skelly->set_sca("useRefl", 0);
        FB.ter->use();
        gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

        pipeLine.use_shader(*VS.modl_skelly);
        { // will become loop
            player.model.mesh->bind_vao();
            VS.modl_skelly->set_glmmat("modelMat", player.model.modelMat);
            VS.modl_skelly->set_glmmat("normMat", to_norm_mat(camera.viewMat * player.model.modelMat));
            VS.modl_skelly->set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
            VS.modl_skelly->set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                FS.modl_frag->set_sca("mode", player.model.skin->get_mode(i));
                player.model.skin->bind_textures(i);
                player.model.mesh->draw_ibo(i);
            }
        }

        pipeLine.use_shader(*VS.modl_static);
        for (auto& model : world.filtered<Model>([](const Model& m)
                { return m.refr; })) {
            model->mesh->bind_vao();
            VS.modl_static->set_glmmat("modelMat", model->modelMat);
            VS.modl_static->set_glmmat("normMat", to_norm_mat(camera.viewMat * model->modelMat));
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                FS.modl_frag->set_sca("mode", model->skin->get_mode(i));
                model->skin->bind_textures(i);
                model->mesh->draw_ibo(i);
            }
        }

        ////////////////////////////////////////

        gl::Disable(gl::CLIP_DISTANCE0);
        FB.pri->use();

        pipeLine.use_shader(*VS.liqd_surface);
        pipeLine.use_shader(*FS.liqd_surface);
        TX.cSec->bind(gl::TEXTURE0);
        TX.cTer->bind(gl::TEXTURE1);
        TX.dTer->bind(gl::TEXTURE2);

        liquid->draw();
    }


    /// Main Rendering ///

    gl::CullFace(gl::BACK);

    pipeLine.use_shader(*FS.modl_frag);

    pipeLine.use_shader(*VS.modl_skelly);
    { // will become loop
        player.model.mesh->bind_vao();
        VS.modl_skelly->set_glmmat("modelMat", player.model.modelMat);
        VS.modl_skelly->set_glmmat("normMat", to_norm_mat(camera.viewMat * player.model.modelMat));
        VS.modl_skelly->set_ptrvec<glm::vec4>("skelQuat", player.model.skeleton.quatData);
        VS.modl_skelly->set_ptrvec<glm::vec3>("skelOffs", player.model.skeleton.offsData);
        for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
            FS.modl_frag->set_sca("mode", player.model.skin->get_mode(i));
            player.model.skin->bind_textures(i);
            player.model.mesh->draw_ibo(i);
        }
    }

    pipeLine.use_shader(*VS.modl_static);
    for (auto& model : world.filtered<Model>()) {
        model->mesh->bind_vao();
        VS.modl_static->set_glmmat("modelMat", model->modelMat);
        VS.modl_static->set_glmmat("normMat", to_norm_mat(camera.viewMat * model->modelMat));
        for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
            FS.modl_frag->set_sca("mode", model->skin->get_mode(i));
            model->skin->bind_textures(i);
            model->mesh->draw_ibo(i);
        }
    }


//    static GLuint vao = 0;
//    static GLuint vbo = 0;
//    static bool fst = true;
//    if (fst) { fst = false;
//        gl::GenBuffers(1, &vbo);
//        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
//        gl::GenVertexArrays(1, &vao);
//        gl::BindVertexArray(vao);
//        gl::EnableVertexAttribArray(0);
//        gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);
//    }

//    pipeLine.disable_stages(gl::FRAGMENT_SHADER_BIT);
//    VS.dbug_lines->set_glmmat("projViewMat", camera.projMat * camera.viewMat);
//    pipeLine.use_shader(*VS.dbug_lines);
//    gl::BindVertexArray(vao);
//    gl::LineWidth(4.f);

//    for (auto& mat : world.skylMatVec) {
//        sq::Frustum f = sq::make_worldFrustum(glm::inverse(mat));
//        float frusData[24*3] {
//            f.xyz.x, f.xyz.y, f.xyz.z,
//            f.xYz.x, f.xYz.y, f.xYz.z,
//            f.xYz.x, f.xYz.y, f.xYz.z,
//            f.XYz.x, f.XYz.y, f.XYz.z,
//            f.XYz.x, f.XYz.y, f.XYz.z,
//            f.Xyz.x, f.Xyz.y, f.Xyz.z,
//            f.Xyz.x, f.Xyz.y, f.Xyz.z,
//            f.xyz.x, f.xyz.y, f.xyz.z,

//            f.xyZ.x, f.xyZ.y, f.xyZ.z,
//            f.xYZ.x, f.xYZ.y, f.xYZ.z,
//            f.xYZ.x, f.xYZ.y, f.xYZ.z,
//            f.XYZ.x, f.XYZ.y, f.XYZ.z,
//            f.XYZ.x, f.XYZ.y, f.XYZ.z,
//            f.XyZ.x, f.XyZ.y, f.XyZ.z,
//            f.XyZ.x, f.XyZ.y, f.XyZ.z,
//            f.xyZ.x, f.xyZ.y, f.xyZ.z,

//            f.xyz.x, f.xyz.y, f.xyz.z,
//            f.xyZ.x, f.xyZ.y, f.xyZ.z,
//            f.xYz.x, f.xYz.y, f.xYz.z,
//            f.xYZ.x, f.xYZ.y, f.xYZ.z,
//            f.XYz.x, f.XYz.y, f.XYz.z,
//            f.XYZ.x, f.XYZ.y, f.XYZ.z,
//            f.Xyz.x, f.Xyz.y, f.Xyz.z,
//            f.XyZ.x, f.XyZ.y, f.XyZ.z
//        };
//        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
//        gl::BufferData(gl::ARRAY_BUFFER, sizeof(frusData), frusData, gl::STREAM_DRAW);
//        gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);
//        gl::DrawArrays(gl::LINES, 0, 24);
//    }


    /// FXAA and/or Screen Output ///

    pipeLine.use_shader(*VS.gnrc_quad);

    gl::Disable(gl::DEPTH_TEST);
    TX.cPri->bind(gl::TEXTURE0);
    int fxaaQuality = app.settings.crnt<int>("fxaaQuality");
    if (fxaaQuality > 0) {
        pipeLine.use_shader(*FS.gnrc_luma);
        sq::draw_screen_quad();
        if (fxaaQuality == 1) pipeLine.use_shader(*FS.post_fxaa_low);
        else pipeLine.use_shader(*FS.post_fxaa_high);
    } else pipeLine.use_shader(*FS.gnrc_passthru);

    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    gl::Clear(gl::COLOR_BUFFER_BIT);
    sq::draw_screen_quad();


    TX.dSkyl->set_param(gl::TEXTURE_COMPARE_MODE, gl::NONE);
    TX.dSkyl->bind(gl::TEXTURE0);
    pipeLine.use_shader(*VS.gnrc_quad);
    pipeLine.use_shader(*FS.gnrc_passthru_layer);
    FS.gnrc_passthru_layer->set_sca("layer", 0);
    sq::draw_grid4x4_quad<sq::Grid4x4::AD>();
    FS.gnrc_passthru_layer->set_sca("layer", 1);
    sq::draw_grid4x4_quad<sq::Grid4x4::BD>();
    FS.gnrc_passthru_layer->set_sca("layer", 2);
    sq::draw_grid4x4_quad<sq::Grid4x4::CD>();
    FS.gnrc_passthru_layer->set_sca("layer", 3);
    sq::draw_grid4x4_quad<sq::Grid4x4::DD>();
    TX.dSkyl->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);

    gl::BindVertexArray(0);
}

bool HandlerGame::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::S) {
            int crnt = app.settings.crnt<int>("shadQuality");
            if      (crnt == 0) app.settings.mod<int>("shadQuality", 1);
            else if (crnt == 1) app.settings.mod<int>("shadQuality", 2);
            else if (crnt == 2) app.settings.mod<int>("shadQuality", 0);
            app.settings.apply();
            return true;
        }
        if (_event.key.code == sf::Keyboard::A) {
            int crnt = app.settings.crnt<int>("fxaaQuality");
            if      (crnt == 0) app.settings.mod<int>("fxaaQuality", 1);
            else if (crnt == 1) app.settings.mod<int>("fxaaQuality", 2);
            else if (crnt == 2) app.settings.mod<int>("fxaaQuality", 0);
            app.settings.apply();
            return true;
        }
    }
    return false;
}
