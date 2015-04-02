#include <list>
#include <map>

#include <SFML/System.hpp>

#include <glm/gtc/matrix_transform.hpp>

#ifdef SQEE_DEBUG
#include <sqee/debug/misc.hpp>
#endif

#include <sqee/app/application.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/misc.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/models/modelstatic.hpp>
#include <sqee/models/modelskelly.hpp>

#include "player.hpp"
#include "maps/world.hpp"
#include "render/camera.hpp"
#include "render/light.hpp"
#include "scenemain.hpp"

inline mat3 to_norm_mat(const mat4& _mvMat) {
    return mat3(glm::transpose(glm::inverse(_mvMat)));
}

using namespace sqt;

SceneMain::SceneMain(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 24;

    camera.reset(new MainCamera());
    player.reset(new Player());
    world.reset(new World());

    camera->pos = {0, 0, 0};
    camera->dir = {0.f, 1.f, 0.f};
    camera->range = {0.25f, 120.f};
    camera->size = {16, 10};
    camera->fov = 1.f;
    camera->binding = 0;
    camera->update();

    player->test_init();
    player->app = &app;
    player->camera = camera.get();
    player->world = world.get();

    world->load_base("WorldTest");

    skylight.reset(new SkyLight());
    skylight->camera = camera.get();

    spotlightVec.emplace_back();
    spotlightVec.back().reset(new SpotLight());
    spotlightVec.back()->baseSize = 256;
    spotlightVec.back()->position = {2.7f, -4.f, 9.5f};
    spotlightVec.back()->angle = glm::radians(50.f) / 2.f;
    spotlightVec.back()->direction = {0.f, 0.3f, -0.7f};
    spotlightVec.back()->intensity = 25.f;
    spotlightVec.back()->colour = {2.f, 2.f, 2.f};
    spotlightVec.back()->softness = 0.8f;
    spotlightVec.back()->camera = camera.get();

//    spotlightVec.emplace_back();
//    spotlightVec.back().reset(new SpotLight());
//    spotlightVec.back()->baseSize = 1024;
//    spotlightVec.back()->position = {19.f, 14.f, 15.f};
//    spotlightVec.back()->angle = glm::radians(70.f) / 2.f;
//    spotlightVec.back()->direction = {0.f, 0.f, -1.f};
//    spotlightVec.back()->intensity = 45.f;
//    spotlightVec.back()->colour = {2.f, 2.f, 2.f};
//    spotlightVec.back()->softness = 0.3f;
//    spotlightVec.back()->camera = &camera;

    FB.spotVec.resize(2);
    TX.spotDepthVec.resize(2);

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
    FB.ssaoA.reset(new sq::Framebuffer());
    FB.ssaoB.reset(new sq::Framebuffer());
    FB.hdr.reset(new sq::Framebuffer());
    FB.basic.reset(new sq::Framebuffer());
    for (auto& fb : FB.skyArr)
        fb.reset(new sq::Framebuffer());
    for (auto& fb : FB.spotVec)
        fb.reset(new sq::Framebuffer());

    /// Create Textures
    TX.defrNorm.reset(new sq::Texture());
    TX.defrSurf.reset(new sq::Texture());
    TX.defrDiff.reset(new sq::Texture());
    TX.defrSpec.reset(new sq::Texture());
    TX.defrDepth.reset(new sq::Texture());
    TX.ssaoGreyA.reset(new sq::Texture());
    TX.ssaoGreyB.reset(new sq::Texture());
    TX.hdrColour.reset(new sq::Texture());
    TX.basicColour.reset(new sq::Texture());
    TX.basicDepth.reset(new sq::Texture());
    TX.skyDepth.reset(new sq::Texture());
    for (auto& tex : TX.spotDepthVec)
        tex.reset(new sq::Texture());

    /// Setup Textures
    using TexPreset = sq::Texture::Preset;
    TX.defrDiff->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, 4, TexPreset::L_C);
    TX.defrNorm->create(gl::TEXTURE_2D, gl::RGB, gl::RGB16, 3, TexPreset::L_C);
    TX.defrSurf->create(gl::TEXTURE_2D, gl::RGB, gl::RGB16, 3, TexPreset::L_C);
    TX.defrSpec->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, 4, TexPreset::L_C);
    TX.defrDepth->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32, 1, TexPreset::L_C);
    TX.ssaoGreyA->create(gl::TEXTURE_2D, gl::RED, gl::R8, 1, TexPreset::L_C);
    TX.ssaoGreyB->create(gl::TEXTURE_2D, gl::RED, gl::R8, 1, TexPreset::L_C);
    TX.hdrColour->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA16F, 4, TexPreset::L_C);
    TX.basicColour->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, 4, TexPreset::L_C);
    TX.basicDepth->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32, 1, TexPreset::L_C);
    TX.skyDepth->create(gl::TEXTURE_2D_ARRAY, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1, TexPreset::L_C);
    TX.skyDepth->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    for (auto& tex : TX.spotDepthVec) {
        tex->create(gl::TEXTURE_2D, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1, TexPreset::L_C);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    }

    /// Setup Framebuffers
    FB.defr->bind();
    FB.defr->attach(gl::COLOR_ATTACHMENT0, *TX.defrDiff);
    FB.defr->attach(gl::COLOR_ATTACHMENT1, *TX.defrNorm);
    FB.defr->attach(gl::COLOR_ATTACHMENT2, *TX.defrSurf);
    FB.defr->attach(gl::COLOR_ATTACHMENT3, *TX.defrSpec);
    FB.defr->attach(gl::DEPTH_ATTACHMENT, *TX.defrDepth);
    FB.ssaoA->bind();
    FB.ssaoA->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoGreyA);
    FB.ssaoB->bind();
    FB.ssaoB->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoGreyB);
    FB.hdr->bind();
    FB.hdr->attach(gl::COLOR_ATTACHMENT0, *TX.hdrColour);
    FB.basic->bind();
    FB.basic->attach(gl::COLOR_ATTACHMENT0, *TX.basicColour);
    FB.basic->attach(gl::DEPTH_ATTACHMENT, *TX.basicDepth);
    for (int i = 0; i < 4; i++) {
        FB.skyArr[i]->bind();
        FB.skyArr[i]->attach_layer(gl::DEPTH_ATTACHMENT, *TX.skyDepth, i);
    }
    for (uint i = 0u; i < spotlightVec.size(); i++) {
        FB.spotVec[i]->bind();
        FB.spotVec[i]->attach(gl::DEPTH_ATTACHMENT, *TX.spotDepthVec[i]);
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
    VS.modl_static->add_uniform("useRefl"); // bool
    VS.modl_skelly->add_uniform("modelMat"); // mat4
    VS.modl_skelly->add_uniform("normMat"); // mat3
    VS.modl_skelly->add_uniform("skelQuat", 40); // vec4
    VS.modl_skelly->add_uniform("skelOffs", 40); // vec3
    VS.modl_skelly->add_uniform("useRefl"); // bool
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
    FS.prty_fxaa_fxaa_low->add_uniform("pixSize"); // vec2
    FS.prty_fxaa_fxaa_high->add_uniform("pixSize"); // vec2
    FS.prty_ssao_ssao_low->add_uniform("pixSize"); // vec2
    FS.prty_ssao_ssao_low->add_uniform("invProjMat"); // mat4
    FS.prty_ssao_ssao_high->add_uniform("pixSize"); // vec2
    FS.prty_ssao_ssao_high->add_uniform("invProjMat"); // mat4
    FS.prty_ssao_blur->add_uniform("pixSize"); // vec2

    pipeLine.reset(new sq::Pipeline());

    camera->recalc_frustums();
}

void SceneMain::resize(uvec2 _size) {
    INFO.fullSize = _size;
    INFO.halfSize = _size / 2u;
    camera->size = vec2(_size);
    vec2 fullPixSize = 1.f / vec2(INFO.fullSize);
    vec2 halfPixSize = 1.f / vec2(INFO.halfSize);

    TX.defrNorm->resize(uvec3(INFO.fullSize, 1));
    TX.defrSurf->resize(uvec3(INFO.fullSize, 1));
    TX.defrDiff->resize(uvec3(INFO.fullSize, 1));
    TX.defrSpec->resize(uvec3(INFO.fullSize, 1));
    TX.defrDepth->resize(uvec3(INFO.fullSize, 1));
    TX.ssaoGreyA->resize(uvec3(INFO.halfSize, 1));
    TX.ssaoGreyB->resize(uvec3(INFO.halfSize, 1));
    TX.hdrColour->resize(uvec3(INFO.fullSize, 1));
    TX.basicColour->resize(uvec3(INFO.fullSize, 1));
    TX.basicDepth->resize(uvec3(INFO.fullSize, 1));

    FS.prty_ssao_ssao_low->set_vec<vec2>("pixSize", halfPixSize);
    FS.prty_ssao_ssao_high->set_vec<vec2>("pixSize", halfPixSize);
    FS.prty_ssao_blur->set_vec<vec2>("pixSize", halfPixSize);
    FS.prty_fxaa_fxaa_low->set_vec<vec2>("pixSize", fullPixSize);
    FS.prty_fxaa_fxaa_high->set_vec<vec2>("pixSize", fullPixSize);
}

void SceneMain::update() {
    world->tick();
    player->tick();
    skylight->update();
    for (auto& spot : spotlightVec)
        spot->update();
}

void SceneMain::render(float _ft) {
    if (app.settings.check_update("SceneGame"))
        update_settings();

    player->calc(accum);
    world->calc(accum);


    /// World Updates ///

        skylight->colour = world->SKYL.colour;
        skylight->direction = world->SKYL.direction;
        FS.shds_ambient->set_vec<vec3>("lightColour", world->ambiColour);
        // spotlight stuff...


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
    // Redo from scratch


    FB.defr->use();
    gl::Viewport(0, 0, app.get_size().x, app.get_size().y);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);


    /// Main Rendering ///

    pipeLine->use_shader(*FS.modl_write);

    pipeLine->use_shader(*VS.modl_skelly);
    { // will become loop
        player->model->mesh->bind_vao();
        VS.modl_skelly->set_mat<mat4>("modelMat", player->model->matrix);
        VS.modl_skelly->set_mat<mat3>("normMat", to_norm_mat(camera->viewMat * player->model->matrix));
        VS.modl_skelly->set_vecptr<vec4>("skelQuat", (float*)player->model->skel.quatVec.data());
        VS.modl_skelly->set_vecptr<vec3>("skelOffs", (float*)player->model->skel.offsVec.data());
        for (uint i = 0; i < player->model->mesh->mCount; i++) {
            FS.modl_write->set_sca<int>("mode", player->model->skin->mtrlVec[i].glMode);
            player->model->skin->bind_textures(i);
            player->model->mesh->draw_ibo(i);
        }
    }

    pipeLine->use_shader(*VS.modl_static);
    for (auto& obj : world->filtered<Model>()) {
        obj->model->mesh->bind_vao();
        VS.modl_static->set_mat<mat4>("modelMat", obj->model->matrix);
        VS.modl_static->set_mat<mat3>("normMat", to_norm_mat(camera->viewMat * obj->model->matrix));
        for (uint i = 0; i < obj->model->mesh->mCount; i++) {
            FS.modl_write->set_sca("mode", obj->model->skin->mtrlVec[i].glMode);
            obj->model->skin->bind_textures(i);
            obj->model->mesh->draw_ibo(i);
        }
    }


    pipeLine->use_shader(*VS.gnrc_quad);
    gl::Disable(gl::DEPTH_TEST);

    TX.defrDiff->bind(gl::TEXTURE0);
    TX.defrNorm->bind(gl::TEXTURE1);
    TX.defrSurf->bind(gl::TEXTURE2);
    TX.defrSpec->bind(gl::TEXTURE3);
    TX.ssaoGreyA->bind(gl::TEXTURE4);
    TX.defrDepth->bind(gl::TEXTURE5);

    /// SSAO ///
    if (INFO.ssaoQuality > 0) {
        if (INFO.ssaoQuality == 1) {
            pipeLine->use_shader(*FS.prty_ssao_ssao_low);
            FS.prty_ssao_ssao_low->set_mat<mat4>("invProjMat", glm::inverse(camera->projMat));
        }
        if (INFO.ssaoQuality == 2) {
            pipeLine->use_shader(*FS.prty_ssao_ssao_high);
            FS.prty_ssao_ssao_high->set_mat<mat4>("invProjMat", glm::inverse(camera->projMat));
        }

        FB.ssaoA->use();
        gl::Viewport(0, 0, INFO.halfSize.x, INFO.halfSize.y);
        sq::draw_screen_quad();

        FB.ssaoB->use();
        pipeLine->use_shader(*FS.prty_ssao_blur);
        sq::draw_screen_quad();
        TX.ssaoGreyB->bind(gl::TEXTURE4);
    }

    FB.hdr->use();
    gl::Viewport(0, 0, INFO.fullSize.x, INFO.fullSize.y);
    gl::Clear(gl::COLOR_BUFFER_BIT);
    gl::BlendFunc(gl::ONE, gl::ONE);
    gl::Enable(gl::BLEND);

    pipeLine->use_shader(*FS.shds_ambient);
    sq::draw_screen_quad();

    skylight->bind_ubo();
    TX.skyDepth->bind(gl::TEXTURE6);
    pipeLine->use_shader(*FS.shds_skylight);
    sq::draw_screen_quad();

    pipeLine->use_shader(*FS.shds_spotlight);
    for (uint ind = 0; ind < spotlightVec.size(); ind++) {
        spotlightVec[ind]->bind_ubo();
        TX.spotDepthVec[ind]->bind(gl::TEXTURE6);
        sq::draw_screen_quad();
    }

    gl::Disable(gl::BLEND);
    TX.hdrColour->bind(gl::TEXTURE0);
    pipeLine->use_shader(*FS.gnrc_lumalpha);
    sq::draw_screen_quad();

    FB.basic->use();
    pipeLine->use_shader(*FS.gnrc_tonemap);
    sq::draw_screen_quad();

    /// FXAA ///
    TX.basicColour->bind(gl::TEXTURE0);
    if (INFO.fxaaQuality > 0) {
        pipeLine->use_shader(*FS.gnrc_lumalpha);
        sq::draw_screen_quad();
        if (INFO.fxaaQuality == 1) pipeLine->use_shader(*FS.prty_fxaa_fxaa_low);
        if (INFO.fxaaQuality == 2) pipeLine->use_shader(*FS.prty_fxaa_fxaa_high);
    } else pipeLine->use_shader(*FS.gnrc_passthru);

    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    sq::draw_screen_quad();



    //pipeLine->use_shader(*VS.gnrc_quad);
    //pipeLine->use_shader(*FS.gnrc_passthru);
    //TX.spotlightVec[0]->set_param(gl::TEXTURE_COMPARE_MODE, gl::NONE);
    //TX.spotlightVec[0]->bind(gl::TEXTURE0);
    //sq::draw_grid4x4_quad<sq::Grid4x4::AD>();
    //TX.spotlightVec[0]->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);



    gl::BindVertexArray(0);
}

void SceneMain::draw_skylight() {
    if (!world->SKYL.enable) return;

    gl::Enable(gl::DEPTH_CLAMP);
    gl::Viewport(0, 0, TX.skyDepth->size.x, TX.skyDepth->size.y);

    for (int ind = 0; ind < 4; ind++) {
        FB.skyArr[ind]->use();
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        VS.shad_static->set_mat<mat4>("shadMat", skylight->matArr[ind]);
        VS.shad_skelly->set_mat<mat4>("shadMat", skylight->matArr[ind]);
        VS.shad_static_punch->set_mat<mat4>("shadMat", skylight->matArr[ind]);
        VS.shad_skelly_punch->set_mat<mat4>("shadMat", skylight->matArr[ind]);

        pipeLine->disable_stages(gl::FRAGMENT_SHADER_BIT);

        // TODO: fix when skel models support punchthrough alpha
        pipeLine->use_shader(*VS.shad_skelly);
        { // will become loop
            VS.shad_skelly->set_mat<mat4>("modelMat", player->model->matrix);
            VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)player->model->skel.quatVec.data());
            VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)player->model->skel.offsVec.data());
            player->model->mesh->bind_vao();
            for (uint i = 0; i < player->model->mesh->mCount; i++) {
                player->model->mesh->draw_ibo(i);
            }
        }

        pipeLine->use_shader(*VS.shad_static);
        for (auto& obj : world->filtered<Model>()) {
            auto fList = obj->model->skin->filtered(0, 1);
            if (fList.empty()) continue;
            VS.shad_static->set_mat<mat4>("modelMat", obj->model->matrix);
            obj->model->mesh->bind_vao();
            for (auto& i : fList) {
                obj->model->mesh->draw_ibo(i);
            }
        }

        pipeLine->use_shader(*VS.shad_static_punch);
        pipeLine->use_shader(*FS.shad_punch);
        for (auto& obj : world->filtered<Model>()) {
            auto fList = obj->model->skin->filtered(1, 1);
            if (fList.empty()) continue;
            VS.shad_static_punch->set_mat<mat4>("modelMat", obj->model->matrix);
            obj->model->mesh->bind_vao();
            for (auto& i : fList) {
                obj->model->skin->bind_textures(i, 1, 0, 0);
                obj->model->mesh->draw_ibo(i);
            }
        }
    }
    gl::Disable(gl::DEPTH_CLAMP);
}

void SceneMain::draw_spotlight() {
    for (uint ind = 0; ind < spotlightVec.size(); ind++) {
        gl::Viewport(0, 0, TX.spotDepthVec[ind]->size.x, TX.spotDepthVec[ind]->size.y);
        FB.spotVec[ind]->use();
        gl::Clear(gl::DEPTH_BUFFER_BIT);
        VS.shad_static->set_mat<mat4>("shadMat", spotlightVec[ind]->matrix);
        VS.shad_skelly->set_mat<mat4>("shadMat", spotlightVec[ind]->matrix);
        VS.shad_static_punch->set_mat<mat4>("shadMat", spotlightVec[ind]->matrix);
        VS.shad_skelly_punch->set_mat<mat4>("shadMat", spotlightVec[ind]->matrix);

        pipeLine->disable_stages(gl::FRAGMENT_SHADER_BIT);

        // TODO: fix when skel models support punchthrough alpha
        pipeLine->use_shader(*VS.shad_skelly);
        { // will become loop
            VS.shad_skelly->set_mat<mat4>("modelMat", player->model->matrix);
            VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)player->model->skel.quatVec.data());
            VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)player->model->skel.offsVec.data());
            player->model->mesh->bind_vao();
            for (uint i = 0; i < player->model->mesh->mCount; i++) {
                player->model->mesh->draw_ibo(i);
            }
        }

        pipeLine->use_shader(*VS.shad_static);
        for (auto& obj : world->filtered<Model>()) {
            auto fList = obj->model->skin->filtered(0, 1);
            if (fList.empty()) continue;
            VS.shad_static->set_mat<mat4>("modelMat", obj->model->matrix);
            obj->model->mesh->bind_vao();
            for (auto& i : fList) {
                obj->model->mesh->draw_ibo(i);
            }
        }

        pipeLine->use_shader(*VS.shad_static_punch);
        pipeLine->use_shader(*FS.shad_punch);
        for (auto& obj : world->filtered<Model>()) {
            auto fList = obj->model->skin->filtered(1, 1);
            if (fList.empty()) continue;
            VS.shad_static_punch->set_mat<mat4>("modelMat", obj->model->matrix);
            obj->model->mesh->bind_vao();
            for (auto& i : fList) {
                obj->model->skin->bind_textures(i, 1, 0, 0);
                obj->model->mesh->draw_ibo(i);
            }
        }
    }
}

void SceneMain::update_settings() {
    INFO.viewDistance = app.settings.crnt<float>("viewDistance");
    INFO.farDistance = app.settings.crnt<float>("farDistance");
    INFO.shadQuality = app.settings.crnt<int>("shadQuality");
    INFO.shadFilter = app.settings.crnt<int>("shadFilter");
    INFO.ssaoQuality = app.settings.crnt<int>("ssaoQuality");
    INFO.fxaaQuality = app.settings.crnt<int>("fxaaQuality");

    camera->range.y = INFO.viewDistance;

    if (INFO.ssaoQuality == 0) {
        FB.ssaoA->use();
        gl::Viewport(0, 0, INFO.halfSize.x, INFO.halfSize.y);
        gl::ClearColor(1.f, 1.f, 1.f, 1.f);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        gl::ClearColor(0.f, 0.f, 0.f, 0.f);
    }

    if (world->SKYL.enable) {
        uint texSize = 1024 * std::pow(2, INFO.shadQuality);
        TX.skyDepth->resize({texSize, texSize, 4});
    }

    for (uint i = 0; i < spotlightVec.size(); i++) {
        uint texSize = spotlightVec[i]->baseSize * std::pow(2, INFO.shadQuality);
        TX.spotDepthVec[i]->resize({texSize, texSize, 1});
    }

    if (INFO.shadFilter == 0) {
        TX.skyDepth->set_preset(sq::Texture::Preset::N_C);
        for (auto& tex : TX.spotDepthVec)
            tex->set_preset(sq::Texture::Preset::N_C);
    } else {
        TX.skyDepth->set_preset(sq::Texture::Preset::L_C);
        for (auto& tex : TX.spotDepthVec)
            tex->set_preset(sq::Texture::Preset::L_C);
    }

    FS.shds_skylight->set_sca("shadQuality", INFO.shadQuality);
    FS.shds_skylight->set_sca("shadFilter", INFO.shadFilter);
    FS.shds_spotlight->set_sca("shadQuality", INFO.shadQuality);
    FS.shds_spotlight->set_sca("shadFilter", INFO.shadFilter);
}


bool HandlerMain::handle(const sf::Event& _event) {
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
