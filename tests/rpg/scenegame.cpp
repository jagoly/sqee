#include <list>
#include <map>

#include <SFML/System.hpp>

#include <glm/gtc/matrix_transform.hpp>

#ifdef SQEE_DEBUG
#include <sqee/debug/miscdraw.hpp>
#endif

#include <sqee/app/application.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/gl/misc.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/models/mesh.hpp>
#include <sqee/models/skin.hpp>

#include "scenegame.hpp"

inline mat3 to_norm_mat(const mat4& _mvMat) {
    return mat3(glm::transpose(glm::inverse(_mvMat)));
}

using namespace sqt;

SceneGame::SceneGame(sq::Application& _app) : sq::Scene(_app), world(_app) {
    tickRate = 24;

    world.load_base("WorldTest");

    camera.pos = {0, 0, 0};
    camera.dir = {0.f, 1.f, 0.f};
    camera.range = {0.25f, 120.f};
    camera.size = {16, 10};
    camera.fov = 1.f;
    camera.binding = 0;
    camera.update();

    player.test_init();
    player.camera = &camera;
    player.world = &world;
    player.settings = &app.settings;

    skylight.camera = &camera;

    //spotlightVec.resize(1);
    spotlightVec.back().position = {10.f, 17.f, 10.f};
    spotlightVec.back().angle = glm::radians(35.f) / 2.f;
    spotlightVec.back().direction = {0.f, 0.f, -1.f};
    spotlightVec.back().intensity = 25.f;
    spotlightVec.back().colour = {2.f, 2.f, 2.f};
    spotlightVec.back().softness = 0.3f;
    FB.spotlightVec.resize(1);
    TX.spotlightVec.resize(1);

    /// Add Settings
    app.settings.add<float>("viewDistance", 120.f);
    app.settings.add<float>("farDistance", 1200.f);
    app.settings.add<int>("shadQuality", 2); // 0=1024, 1=2048, 2=4096
    app.settings.add<int>("shadFilter", 2); // 0=Off, 1=Minimal, 2=Full
    app.settings.add<int>("ssaoQuality", 2); // 0=Off, 1=Low, 2=High
    app.settings.add<int>("fxaaQuality", 2); // 0=Off, 1=Low, 2=High
    app.settings.add<bool>("mouseFocus", true);

    /// Create Framebuffers
    FB.defr.reset(new sq::Framebuffer());
    FB.hdr.reset(new sq::Framebuffer());
    FB.pri.reset(new sq::Framebuffer());
    FB.sec.reset(new sq::Framebuffer());
    FB.ssao.reset(new sq::Framebuffer());
    for (auto& fb : FB.skylightArr)
        fb.reset(new sq::Framebuffer());
    for (auto& fb : FB.spotlightVec)
        fb.reset(new sq::Framebuffer());

    /// Create Textures
    TX.defrNorm.reset(new sq::Texture());
    TX.defrSurf.reset(new sq::Texture());
    TX.defrDiff.reset(new sq::Texture());
    TX.defrSpec.reset(new sq::Texture());
    TX.defrDep.reset(new sq::Texture());
    TX.hdrFloat.reset(new sq::Texture());
    TX.priCol.reset(new sq::Texture());
    TX.priDep.reset(new sq::Texture());
    TX.secCol.reset(new sq::Texture());
    TX.secDep.reset(new sq::Texture());
    TX.ssao.reset(new sq::Texture());
    TX.skylight.reset(new sq::Texture());
    for (auto& tex : TX.spotlightVec)
        tex.reset(new sq::Texture());

    /// Setup Textures
    using TexPreset = sq::Texture::Preset;
    TX.defrNorm->create(gl::TEXTURE_2D, gl::RGB, gl::RGB16, 3, TexPreset::L_C);
    TX.defrSurf->create(gl::TEXTURE_2D, gl::RGB, gl::RGB16, 3, TexPreset::L_C);
    TX.defrDiff->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, 4, TexPreset::L_C);
    TX.defrSpec->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, 4, TexPreset::L_C);
    TX.defrDep->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32, 1, TexPreset::L_C);
    TX.hdrFloat->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA16F, 4, TexPreset::L_C);
    TX.priCol->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, 4, TexPreset::L_C);
    TX.priDep->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32F, 1, TexPreset::L_C);
    TX.secCol->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, 4, TexPreset::L_C);
    TX.secDep->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32F, 1, TexPreset::L_C);
    TX.ssao->create(gl::TEXTURE_2D, gl::RG, gl::RG8, 2, TexPreset::L_C);
    TX.skylight->create(gl::TEXTURE_2D_ARRAY, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1, TexPreset::L_C);
    TX.skylight->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    for (auto& tex : TX.spotlightVec) {
        tex->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1, TexPreset::L_C);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    }

    /// Setup Framebuffers
    FB.defr->bind();
    FB.defr->attach(gl::COLOR_ATTACHMENT0, *TX.defrNorm);
    FB.defr->attach(gl::COLOR_ATTACHMENT1, *TX.defrSurf);
    FB.defr->attach(gl::COLOR_ATTACHMENT2, *TX.defrDiff);
    FB.defr->attach(gl::COLOR_ATTACHMENT3, *TX.defrSpec);
    FB.defr->attach(gl::DEPTH_ATTACHMENT, *TX.defrDep);
    FB.hdr->bind();
    FB.hdr->attach(gl::COLOR_ATTACHMENT0, *TX.hdrFloat);
    FB.pri->bind();
    FB.pri->attach(gl::COLOR_ATTACHMENT0, *TX.priCol);
    FB.pri->attach(gl::DEPTH_ATTACHMENT, *TX.priDep);
    FB.sec->bind();
    FB.sec->attach(gl::COLOR_ATTACHMENT0, *TX.secCol);
    FB.sec->attach(gl::DEPTH_ATTACHMENT, *TX.secDep);
    FB.ssao->bind();
    FB.ssao->attach(gl::COLOR_ATTACHMENT0, *TX.ssao);
    for (int i = 0; i < 4; i++) {
        FB.skylightArr[i]->bind();
        FB.skylightArr[i]->attach_layer(gl::DEPTH_ATTACHMENT, *TX.skylight, i);
    }
    for (uint i = 0u; i < spotlightVec.size(); i++) {
        FB.spotlightVec[i]->bind();
        FB.spotlightVec[i]->attach(gl::DEPTH_ATTACHMENT, *TX.spotlightVec[i]);
    }

    /// Import GLSL Headers
    app.preproc.import_header("headers/blocks/camera");
    app.preproc.import_header("headers/blocks/world");
    app.preproc.import_header("headers/blocks/liquid");
    app.preproc.import_header("headers/blocks/skylight");
    app.preproc.import_header("headers/blocks/spotlight");
    app.preproc.import_header("headers/uniform_disks");
    app.preproc.import_header("headers/shadow/sample_sky");
    app.preproc.import_header("headers/shadow/sample_spot");

    /// Create Shaders
    VS.gnrc_quad.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gnrc_lines.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.gnrc_passthru.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_passthru_layer.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_lumalpha.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_tonemap.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.modl_static.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.modl_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.modl_write.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_ambient.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_skylight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spotlight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    //VS.liqd_surface.reset(new sq::Shader(gl::VERTEX_SHADER));
    //FS.liqd_surface.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.shad_static.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_static_punch.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_skelly_punch.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.shad_punch.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_fxaa_fxaa_low.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_fxaa_fxaa_high.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_ssao_ssao_low.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_ssao_ssao_high.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_ssao_blur.reset(new sq::Shader(gl::FRAGMENT_SHADER));


    /// Load Shaders
    VS.gnrc_quad->load(app.preproc("generic/quad_vs"));
    VS.gnrc_lines->load(app.preproc("generic/lines_vs"));
    FS.gnrc_passthru->load(app.preproc("generic/passthru_fs"));
    FS.gnrc_passthru_layer->load(app.preproc("generic/passthru_fs", "#define LAYER"));
    FS.gnrc_lumalpha->load(app.preproc("generic/lumalpha_fs"));
    FS.gnrc_tonemap->load(app.preproc("generic/tonemap_fs"));
    VS.modl_static->load(app.preproc("models/static_vs"));
    VS.modl_skelly->load(app.preproc("models/skelly_vs"));
    FS.modl_write->load(app.preproc("models/write_fs"));
    FS.shds_ambient->load(app.preproc("shades/ambient_fs"));
    FS.shds_skylight->load(app.preproc("shades/skylight_fs"));
    FS.shds_spotlight->load(app.preproc("shades/spotlight_fs"));
    //VS.liqd_surface->load(app.preproc("liquids/surface_vs"));
    //FS.liqd_surface->load(app.preproc("liquids/surface_fs"));
    VS.shad_static->load(app.preproc("shadows/static_vs"));
    VS.shad_skelly->load(app.preproc("shadows/skelly_vs"));
    VS.shad_static_punch->load(app.preproc("shadows/static_vs", "#define PUNCH"));
    VS.shad_skelly_punch->load(app.preproc("shadows/skelly_vs", "#define PUNCH"));
    FS.shad_punch->load(app.preproc("shadows/punch_fs"));
    FS.prty_fxaa_fxaa_low->load(app.preproc("pretty/fxaa/fxaa_fs", "#define FXAA_QUALITY__PRESET 10"));
    FS.prty_fxaa_fxaa_high->load(app.preproc("pretty/fxaa/fxaa_fs", "#define FXAA_QUALITY__PRESET 39"));
    FS.prty_ssao_ssao_low->load(app.preproc("pretty/ssao/ssao_fs"));
    FS.prty_ssao_ssao_high->load(app.preproc("pretty/ssao/ssao_fs", "#define HIGH"));
    FS.prty_ssao_blur->load(app.preproc("pretty/ssao/blur_fs"));

    /// Add Uniforms to Shaders
    VS.gnrc_lines->add_uniform("projViewMat"); // mat4
    FS.gnrc_passthru_layer->add_uniform("layer"); // int
    VS.modl_static->add_uniform("modelMat"); // mat4
    VS.modl_static->add_uniform("normMat"); // mat3
    VS.modl_static->add_uniform("useRefl"); // int
    VS.modl_skelly->add_uniform("modelMat"); // mat4
    VS.modl_skelly->add_uniform("normMat"); // mat3
    VS.modl_skelly->add_uniform("skelQuat", 40); // vec4
    VS.modl_skelly->add_uniform("skelOffs", 40); // vec3
    VS.modl_skelly->add_uniform("useRefl"); // int
    FS.shds_ambient->add_uniform("lightColour"); // vec3
    VS.shad_static->add_uniform("shadMat"); // mat4
    VS.shad_static->add_uniform("modelMat"); // mat4
    VS.shad_skelly->add_uniform("shadMat"); // mat4
    VS.shad_skelly->add_uniform("modelMat"); // mat4
    VS.shad_skelly->add_uniform("skelQuat", 40); // vec4
    VS.shad_skelly->add_uniform("skelOffs", 40); // vec3
    FS.modl_write->add_uniform("mode"); // int
    FS.shds_skylight->add_uniform("shadQuality"); // int
    FS.shds_skylight->add_uniform("shadFilter"); // int
    FS.shds_spotlight->add_uniform("shadQuality"); // int
    FS.shds_spotlight->add_uniform("shadFilter"); // int
    VS.shad_static_punch->add_uniform("shadMat"); // mat4
    VS.shad_static_punch->add_uniform("modelMat"); // mat4
    VS.shad_skelly_punch->add_uniform("shadMat"); // mat4
    VS.shad_skelly_punch->add_uniform("modelMat"); // mat4
    VS.shad_skelly_punch->add_uniform("skelQuat", 40); // vec4
    VS.shad_skelly_punch->add_uniform("skelOffs", 40); // vec3
    FS.prty_fxaa_fxaa_low->add_uniform("vpPixSize"); // vec2
    FS.prty_fxaa_fxaa_high->add_uniform("vpPixSize"); // vec2
    FS.prty_ssao_ssao_low->add_uniform("viewSize"); // vec2
    FS.prty_ssao_ssao_low->add_uniform("invProjMat"); // mat4
    FS.prty_ssao_ssao_high->add_uniform("viewSize"); // vec2
    FS.prty_ssao_ssao_high->add_uniform("invProjMat"); // mat4
    FS.prty_ssao_blur->add_uniform("viewSize"); // vec2

    pipeLine.reset(new sq::Pipeline());

    camera.recalc_frustums();
}

void SceneGame::resize(uvec2 _size) {
    camera.size = vec2(_size);

    TX.defrNorm->resize({_size.x, _size.y, 1});
    TX.defrSurf->resize({_size.x, _size.y, 1});
    TX.defrDiff->resize({_size.x, _size.y, 1});
    TX.defrSpec->resize({_size.x, _size.y, 1});
    TX.defrDep->resize({_size.x, _size.y, 1});
    TX.hdrFloat->resize({_size.x, _size.y, 1});
    TX.priCol->resize({_size.x, _size.y, 1});
    TX.priDep->resize({_size.x, _size.y, 1});
    TX.secCol->resize({_size.x, _size.y, 1});
    TX.secDep->resize({_size.x, _size.y, 1});

    vec2 pixSize(1.f / _size.x, 1.f / _size.y);
    FS.prty_fxaa_fxaa_low->set_glmvec("vpPixSize", pixSize);
    FS.prty_fxaa_fxaa_high->set_glmvec("vpPixSize", pixSize);

    uvec2 halfSize = uvec2(vec2(_size)/2.f);
    TX.ssao->resize({halfSize.x, halfSize.y, 1});
    FS.prty_ssao_ssao_low->set_glmvec("viewSize", vec2(halfSize));
    FS.prty_ssao_ssao_high->set_glmvec("viewSize", vec2(halfSize));
    FS.prty_ssao_blur->set_glmvec("viewSize", vec2(halfSize));
}

void SceneGame::update() {
    world.tick();

    static std::list<sq::Direction> keys;
    static std::map<sq::Direction, bool> keyStates {
        {sq::Direction::North, false}, {sq::Direction::East, false},
        {sq::Direction::South, false}, {sq::Direction::West, false}
    };
    std::map<sq::Direction, bool> keyStatesPrev = keyStates;

    keyStates[sq::Direction::North] = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    keyStates[sq::Direction::East]  = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    keyStates[sq::Direction::South] = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    keyStates[sq::Direction::West]  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);

    for (auto& dpPair : keyStates) {
        if (dpPair.second && !keyStatesPrev.at(dpPair.first)) {
            keys.remove(dpPair.first); keys.emplace_front(dpPair.first);
        } else
        if (!dpPair.second && keyStatesPrev.at(dpPair.first)) {
            keys.remove(dpPair.first);
        }
    }

    player.attempt_move(keys.empty() ? sq::Direction::Zero : keys.front());
    player.tick();
    skylight.update();
    spotlightVec[0].update();
}

void SceneGame::render(float _ft) {
    if (app.settings.check_update("SceneGame"))
        update_settings();

    player.calc(accum);
    world.calc(accum);


    /// World Updates ///

    if (world.updateScene) { world.updateScene = false;
        skylight.colour = world.SKYL.colour;
        skylight.direction = world.SKYL.direction;
        FS.shds_ambient->set_glmvec("lightColour", world.ambiColour);
        // spotlight stuff...
    }


    /// Per-frame setup ///
    pipeLine->bind();
    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    gl::Enable(gl::CULL_FACE);
    gl::CullFace(gl::BACK);


    /// Light Shadows ///
    draw_skylight();
    draw_spotlight();


    /// Liquids and submerged areas ///

    FB.pri->use();
    gl::Viewport(0, 0, app.get_size().x, app.get_size().y);

    // TEMP
/*    gl::ClearColor(0.f, 0.1f, 0.2f, 1.f);

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
        TX.secCol->bind(gl::TEXTURE0);
        TX.terCol->bind(gl::TEXTURE1);
        TX.terDep->bind(gl::TEXTURE2);

        liquid->draw();
    }
*/


    FB.defr->use();
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);


    /// Main Rendering ///

    pipeLine->use_shader(*FS.modl_write);

    pipeLine->use_shader(*VS.modl_skelly);
    { // will become loop
        player.model.mesh->bind_vao();
        VS.modl_skelly->set_glmmat<mat4>("modelMat", player.model.modelMat);
        VS.modl_skelly->set_glmmat<mat3>("normMat", to_norm_mat(camera.viewMat * player.model.modelMat));
        VS.modl_skelly->set_ptrvec<vec4>("skelQuat", player.model.skeleton.quatData);
        VS.modl_skelly->set_ptrvec<vec3>("skelOffs", player.model.skeleton.offsData);
        for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
            FS.modl_write->set_sca<int>("mode", player.model.skin->get_mode(i));
            player.model.skin->bind_textures(i);
            player.model.mesh->draw_ibo(i);
        }
    }

    pipeLine->use_shader(*VS.modl_static);
    for (auto& model : world.filtered<Model>()) {
        model->mesh->bind_vao();
        VS.modl_static->set_glmmat<mat4>("modelMat", model->modelMat);
        VS.modl_static->set_glmmat<mat3>("normMat", to_norm_mat(camera.viewMat * model->modelMat));
        for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
            FS.modl_write->set_sca("mode", model->skin->get_mode(i));
            model->skin->bind_textures(i);
            model->mesh->draw_ibo(i);
        }
    }

    pipeLine->use_shader(*VS.gnrc_quad);
    gl::Disable(gl::DEPTH_TEST);

    /// SSAO ///
    FB.ssao->use();
    gl::Viewport(0, 0, TX.ssao->size.x, TX.ssao->size.y);
    int ssaoSetting = app.settings.crnt<int>("ssaoQuality");
    if (ssaoSetting > 0) {
        TX.defrSurf->bind(gl::TEXTURE0);
        TX.defrDep->bind(gl::TEXTURE1);
        if (ssaoSetting == 1) {
            pipeLine->use_shader(*FS.prty_ssao_ssao_low);
            FS.prty_ssao_ssao_low->set_glmmat<mat4>("invProjMat", glm::inverse(camera.projMat));
        }
        if (ssaoSetting == 2) {
            pipeLine->use_shader(*FS.prty_ssao_ssao_high);
            FS.prty_ssao_ssao_high->set_glmmat<mat4>("invProjMat", glm::inverse(camera.projMat));
        }
        sq::draw_screen_quad();
        TX.ssao->bind(gl::TEXTURE0);
        pipeLine->use_shader(*FS.prty_ssao_blur);
        sq::draw_screen_quad();
    } else {
        gl::ClearColor(1.f, 1.f, 1.f, 1.f);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        gl::ClearColor(0.f, 0.f, 0.f, 0.f);
    }

    TX.defrNorm->bind(gl::TEXTURE0);
    TX.defrSurf->bind(gl::TEXTURE1);
    TX.defrDiff->bind(gl::TEXTURE2);
    TX.defrSpec->bind(gl::TEXTURE3);
    TX.ssao->bind(gl::TEXTURE4);
    TX.defrDep->bind(gl::TEXTURE5);

    FB.hdr->use();
    gl::Viewport(0, 0, app.get_size().x, app.get_size().y);
    gl::Clear(gl::COLOR_BUFFER_BIT);
    gl::BlendFunc(gl::ONE, gl::ONE);
    gl::Enable(gl::BLEND);

    pipeLine->use_shader(*FS.shds_ambient);
    sq::draw_screen_quad();

    skylight.bind_ubo();
    TX.skylight->bind(gl::TEXTURE6);
    pipeLine->use_shader(*FS.shds_skylight);
    sq::draw_screen_quad();

    spotlightVec[0].bind_ubo();
    TX.spotlightVec[0]->bind(gl::TEXTURE6);
    pipeLine->use_shader(*FS.shds_spotlight);
    sq::draw_screen_quad();

    gl::Disable(gl::BLEND);
    TX.hdrFloat->bind(gl::TEXTURE0);
    pipeLine->use_shader(*FS.gnrc_lumalpha);
    sq::draw_screen_quad();

    FB.pri->use();
    pipeLine->use_shader(*FS.gnrc_tonemap);
    sq::draw_screen_quad();

    /// FXAA ///
    int fxaaSetting = app.settings.crnt<int>("fxaaQuality");
    TX.priCol->bind(gl::TEXTURE0);
    if (fxaaSetting > 0) {
        pipeLine->use_shader(*FS.gnrc_lumalpha);
        sq::draw_screen_quad();
        if (fxaaSetting == 1) pipeLine->use_shader(*FS.prty_fxaa_fxaa_low);
        if (fxaaSetting == 2) pipeLine->use_shader(*FS.prty_fxaa_fxaa_high);
    } else pipeLine->use_shader(*FS.gnrc_passthru);

    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    sq::draw_screen_quad();



    pipeLine->use_shader(*VS.gnrc_quad);
    pipeLine->use_shader(*FS.gnrc_passthru);
    TX.spotlightVec[0]->set_param(gl::TEXTURE_COMPARE_MODE, gl::NONE);
    TX.spotlightVec[0]->bind(gl::TEXTURE0);
    sq::draw_grid4x4_quad<sq::Grid4x4::AD>();
    TX.spotlightVec[0]->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);



    gl::BindVertexArray(0);
}

void SceneGame::draw_skylight() {
    if (!world.SKYL.enable) return;

    gl::Enable(gl::DEPTH_CLAMP);
    gl::Viewport(0, 0, TX.skylight->size.x, TX.skylight->size.y);

    for (int ind = 0; ind < 4; ind++) {
        FB.skylightArr[ind]->use();
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        VS.shad_static->set_glmmat<mat4>("shadMat", skylight.matArr[ind]);
        VS.shad_skelly->set_glmmat<mat4>("shadMat", skylight.matArr[ind]);
        VS.shad_static_punch->set_glmmat<mat4>("shadMat", skylight.matArr[ind]);
        VS.shad_skelly_punch->set_glmmat<mat4>("shadMat", skylight.matArr[ind]);

        pipeLine->disable_stages(gl::FRAGMENT_SHADER_BIT);

        // TODO: fix when skel models support punchthrough alpha
        pipeLine->use_shader(*VS.shad_skelly);
        { // will become loop
            VS.shad_skelly->set_glmmat<mat4>("modelMat", player.model.modelMat);
            VS.shad_skelly->set_ptrvec<vec4>("skelQuat", player.model.skeleton.quatData);
            VS.shad_skelly->set_ptrvec<vec3>("skelOffs", player.model.skeleton.offsData);
            player.model.mesh->bind_vao();
            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                player.model.mesh->draw_ibo(i);
            }
        }

        pipeLine->use_shader(*VS.shad_static);
        for (auto& model : world.filtered<Model>([](const Model& m)
                { return m.shad && !m.pnch; })) {
            VS.shad_static->set_glmmat<mat4>("modelMat", model->modelMat);
            model->mesh->bind_vao();
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                model->mesh->draw_ibo(i);
            }
        }

        pipeLine->use_shader(*VS.shad_static_punch);
        pipeLine->use_shader(*FS.shad_punch);
        for (auto& model : world.filtered<Model>([](const Model& m)
                { return m.shad && m.pnch; })) {
            VS.shad_static_punch->set_glmmat<mat4>("modelMat", model->modelMat);
            model->mesh->bind_vao();
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                model->skin->bind_textures(i, sq::Skin::Mode::diff);
                model->mesh->draw_ibo(i);
            }
        }
    }
    gl::Disable(gl::DEPTH_CLAMP);
}

void SceneGame::draw_spotlight() {
    for (uint ind = 0; ind < spotlightVec.size(); ind++) {
        gl::Viewport(0, 0, TX.spotlightVec[ind]->size.x, TX.spotlightVec[ind]->size.y);
        FB.spotlightVec[ind]->use();
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        VS.shad_static->set_glmmat<mat4>("shadMat", spotlightVec[ind].matrix);
        VS.shad_skelly->set_glmmat<mat4>("shadMat", spotlightVec[ind].matrix);
        VS.shad_static_punch->set_glmmat<mat4>("shadMat", spotlightVec[ind].matrix);
        VS.shad_skelly_punch->set_glmmat<mat4>("shadMat", spotlightVec[ind].matrix);

        pipeLine->disable_stages(gl::FRAGMENT_SHADER_BIT);

        // TODO: fix when skel models support punchthrough alpha
        pipeLine->use_shader(*VS.shad_skelly);
        { // will become loop
            VS.shad_skelly->set_glmmat<mat4>("modelMat", player.model.modelMat);
            VS.shad_skelly->set_ptrvec<vec4>("skelQuat", player.model.skeleton.quatData);
            VS.shad_skelly->set_ptrvec<vec3>("skelOffs", player.model.skeleton.offsData);
            player.model.mesh->bind_vao();
            for (uint i = 0; i < player.model.mesh->iboVec.size(); i++) {
                player.model.mesh->draw_ibo(i);
            }
        }

        pipeLine->use_shader(*VS.shad_static);
        for (auto& model : world.filtered<Model>([](const Model& m)
                { return m.shad && !m.pnch; })) {
            VS.shad_static->set_glmmat<mat4>("modelMat", model->modelMat);
            model->mesh->bind_vao();
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                model->mesh->draw_ibo(i);
            }
        }

        pipeLine->use_shader(*VS.shad_static_punch);
        pipeLine->use_shader(*FS.shad_punch);
        for (auto& model : world.filtered<Model>([](const Model& m)
                { return m.shad && m.pnch; })) {
            VS.shad_static_punch->set_glmmat<mat4>("modelMat", model->modelMat);
            model->mesh->bind_vao();
            for (uint i = 0; i < model->mesh->iboVec.size(); i++) {
                model->skin->bind_textures(i, sq::Skin::Mode::diff);
                model->mesh->draw_ibo(i);
            }
        }
    }
}

void SceneGame::update_settings() {
    float viewDistance = app.settings.crnt<float>("viewDistance");
    int shadQuality = app.settings.crnt<int>("shadQuality");
    int shadFilter = app.settings.crnt<int>("shadFilter");

    camera.range.y = viewDistance;

    if (world.SKYL.enable) {
        uint texSize = 1024 * std::pow(2, shadQuality);
        TX.skylight->resize({texSize, texSize, 4});
    }

    for (uint i = 0; i < spotlightVec.size(); i++) {
        uint texSize = spotlightVec[i].baseSize * std::pow(2, shadQuality);
        TX.spotlightVec[i]->resize({texSize, texSize, 1});
    }

    if (shadFilter == 0) {
        TX.skylight->set_preset(sq::Texture::Preset::N_C);
        for (auto& tex : TX.spotlightVec)
            tex->set_preset(sq::Texture::Preset::N_C);
    } else {
        TX.skylight->set_preset(sq::Texture::Preset::L_C);
        for (auto& tex : TX.spotlightVec)
            tex->set_preset(sq::Texture::Preset::L_C);
    }

    FS.shds_skylight->set_sca("shadQuality", shadQuality);
    FS.shds_skylight->set_sca("shadFilter", shadFilter);
    FS.shds_spotlight->set_sca("shadQuality", shadQuality);
    FS.shds_spotlight->set_sca("shadFilter", shadFilter);
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
        if (_event.key.code == sf::Keyboard::F) {
            int crnt = app.settings.crnt<int>("shadFilter");
            if      (crnt == 0) app.settings.mod<int>("shadFilter", 1);
            else if (crnt == 1) app.settings.mod<int>("shadFilter", 2);
            else if (crnt == 2) app.settings.mod<int>("shadFilter", 0);
            app.settings.apply();
            return true;
        }
        if (_event.key.code == sf::Keyboard::O) {
            int crnt = app.settings.crnt<int>("ssaoQuality");
            if      (crnt == 0) app.settings.mod<int>("ssaoQuality", 1);
            else if (crnt == 1) app.settings.mod<int>("ssaoQuality", 2);
            else if (crnt == 2) app.settings.mod<int>("ssaoQuality", 0);
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

    if (_event.type == sf::Event::MouseButtonPressed) {
        if (_event.mouseButton.button == sf::Mouse::Left) {
            bool crnt = app.settings.crnt<bool>("mouseFocus");
            app.settings.mod<bool>("mouseFocus", !crnt);
            app.settings.apply();
            return true;
        }
    }
    return false;
}
