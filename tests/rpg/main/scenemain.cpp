#include <list>
#include <map>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#ifdef SQEE_DEBUG
#include <sqee/debug/misc.hpp>
#include <sqee/debug/glcallback.hpp>
#endif

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/application.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/misc.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/cameras.hpp>
#include <sqee/render/lightsky.hpp>

#include "../rndr/graph.hpp"
#include "../wcoe/world.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/lightsky.hpp"
#include "../wcoe/obj/lightspot.hpp"
#include "../wcoe/obj/lightpoint.hpp"
#include "camera.hpp"
#include "scenemain.hpp"

using namespace sqt;

SceneMain::SceneMain(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 24;

    camera.reset(new MainCamera());
    world.reset(new wcoe::World());
    world->camera = camera.get();
    world->settings = &app.settings;
    graph.reset(new rndr::Graph());
    graph->camera = camera.get();
    graph->settings = &app.settings;
    pipeline.reset(new sq::Pipeline());

    camera->pos = {0, -1, 1};
    camera->dir = {0.1, 0.8, -0.1};
    camera->range = {0.25f, 120.f};
    camera->size = {16.f, 10.f};
    camera->fov = 1.f;
    camera->binding = 0;
    camera->update();

    world->add_cell("GLOBAL", {0,0,0});
    world->add_cell("Sponza", {0,0,0}, "Sponza/New");
    world->enable_cell("GLOBAL");
    world->enable_cell("Sponza");


    /// Player Model


    /// Add Settings
    app.settings.add<float>("viewDistance", 120.f);
    app.settings.add<float>("farDistance", 1200.f);
    app.settings.add<int>("shadQuality", 2); // 0=1024, 1=2048, 2=4096
    app.settings.add<int>("shadFilter", 2); // 0=Off, 1=Minimal, 2=Full
    app.settings.add<int>("ssaoQuality", 2); // 0=Off, 1=Low, 2=High
    app.settings.add<int>("fxaaQuality", 2); // 0=Off, 1=Low, 2=High
    app.settings.add<bool>("mouseFocus", true);
    app.settings.add<bool>("vignetting", true);

    /// Create Framebuffers
    FB.defr.reset(new sq::Framebuffer());
    FB.ssaoA.reset(new sq::Framebuffer());
    FB.ssaoB.reset(new sq::Framebuffer());
    FB.hdr.reset(new sq::Framebuffer());
    FB.basic.reset(new sq::Framebuffer());

    /// Create Textures
    TX.defrNorm.reset(new sq::Texture2D());
    TX.defrSurf.reset(new sq::Texture2D());
    TX.defrDiff.reset(new sq::Texture2D());
    TX.defrSpec.reset(new sq::Texture2D());
    TX.defrDepth.reset(new sq::Texture2D());
    TX.ssaoGreyA.reset(new sq::Texture2D());
    TX.ssaoGreyB.reset(new sq::Texture2D());
    TX.hdrColour.reset(new sq::Texture2D());
    TX.basicColour.reset(new sq::Texture2D());
    TX.basicDepth.reset(new sq::Texture2D());

    /// Setup Textures
    TX.defrDiff->create(gl::RGBA, gl::RGBA8, 4);
    TX.defrNorm->create(gl::RGB, gl::RGB16, 3);
    TX.defrSurf->create(gl::RGB, gl::RGB16, 3);
    TX.defrSpec->create(gl::RGBA, gl::RGBA8, 4);
    TX.defrDepth->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32, 1);
    TX.ssaoGreyA->create(gl::RED, gl::R8, 1);
    TX.ssaoGreyB->create(gl::RED, gl::R8, 1);
    TX.hdrColour->create(gl::RGBA, gl::RGBA16F, 4);
    TX.basicColour->create(gl::RGBA, gl::RGBA8, 4);
    TX.basicDepth->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32, 1);
    TX.defrDiff->set_preset(sq::Texture2D::L_C());
    TX.defrNorm->set_preset(sq::Texture2D::L_C());
    TX.defrSurf->set_preset(sq::Texture2D::L_C());
    TX.defrSpec->set_preset(sq::Texture2D::L_C());
    TX.defrDepth->set_preset(sq::Texture2D::L_C());
    TX.ssaoGreyA->set_preset(sq::Texture2D::L_C());
    TX.ssaoGreyB->set_preset(sq::Texture2D::L_C());
    TX.hdrColour->set_preset(sq::Texture2D::L_C());
    TX.basicColour->set_preset(sq::Texture2D::L_C());
    TX.basicDepth->set_preset(sq::Texture2D::L_C());

    /// Setup Framebuffers
    FB.defr->attach(gl::COLOR_ATTACHMENT0, *TX.defrDiff);
    FB.defr->attach(gl::COLOR_ATTACHMENT1, *TX.defrNorm);
    FB.defr->attach(gl::COLOR_ATTACHMENT2, *TX.defrSurf);
    FB.defr->attach(gl::COLOR_ATTACHMENT3, *TX.defrSpec);
    FB.defr->attach(gl::DEPTH_ATTACHMENT, *TX.defrDepth);
    FB.ssaoA->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoGreyA);
    FB.ssaoB->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoGreyB);
    FB.hdr->attach(gl::COLOR_ATTACHMENT0, *TX.hdrColour);
    FB.basic->attach(gl::COLOR_ATTACHMENT0, *TX.basicColour);
    FB.basic->attach(gl::DEPTH_ATTACHMENT, *TX.basicDepth);

    /// Import GLSL Headers
    app.preproc.import_header("headers/blocks/camera");
    app.preproc.import_header("headers/blocks/world");
    app.preproc.import_header("headers/blocks/liquid");
    app.preproc.import_header("headers/blocks/skylight");
    app.preproc.import_header("headers/blocks/spotlight");
    app.preproc.import_header("headers/blocks/pointlight");
    app.preproc.import_header("headers/uniform_disks");
    app.preproc.import_header("headers/shadow/sample_sky");
    app.preproc.import_header("headers/shadow/sample_spot");
    app.preproc.import_header("headers/shadow/sample_point");

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
    FS.shds_pointlight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
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
    FS.prty_vignette.reset(new sq::Shader(gl::FRAGMENT_SHADER));

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
    FS.shds_pointlight->load(app.preproc("shades/pointlight_fs"));
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
    FS.prty_vignette->load(app.preproc("pretty/vignette/vignette_fs"));

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
    VS.shad_static->add_uniform("matrix"); // mat4
    VS.shad_skelly->add_uniform("matrix"); // mat4
    VS.shad_skelly->add_uniform("skelQuat", 40); // vec4
    VS.shad_skelly->add_uniform("skelOffs", 40); // vec3
    VS.shad_static_punch->add_uniform("matrix"); // mat4
    VS.shad_skelly_punch->add_uniform("matrix"); // mat4
    VS.shad_skelly_punch->add_uniform("skelQuat", 40); // vec4
    VS.shad_skelly_punch->add_uniform("skelOffs", 40); // vec3
    FS.modl_write->add_uniform("mode"); // int
    FS.shds_skylight->add_uniform("shadQuality"); // int
    FS.shds_skylight->add_uniform("shadFilter"); // int
    FS.shds_spotlight->add_uniform("shadQuality"); // int
    FS.shds_spotlight->add_uniform("shadFilter"); // int
    FS.shds_pointlight->add_uniform("shadQuality"); // int
    FS.shds_pointlight->add_uniform("shadFilter"); // int
    FS.prty_fxaa_fxaa_low->add_uniform("pixSize"); // vec2
    FS.prty_fxaa_fxaa_high->add_uniform("pixSize"); // vec2
    FS.prty_ssao_ssao_low->add_uniform("pixSize"); // vec2
    FS.prty_ssao_ssao_high->add_uniform("pixSize"); // vec2
    FS.prty_ssao_blur->add_uniform("pixSize"); // vec2


    for (const auto& model : world->filtered<wcoe::ModelStatic>())
        graph->modelStaticList.push_back(model);
    for (const auto& model : world->filtered<wcoe::ModelSkelly>())
        graph->modelSkellyList.emplace_back(model);
    for (const auto& light : world->filtered<wcoe::LightSky>())
        graph->lightSkyList.emplace_back(light);
    for (const auto& light : world->filtered<wcoe::LightSpot>())
        graph->lightSpotList.emplace_back(light);
    for (const auto& light : world->filtered<wcoe::LightPoint>())
        graph->lightPointList.emplace_back(light);

    graph->VS.modl_static = VS.modl_static.get();
    graph->VS.modl_skelly = VS.modl_skelly.get();
    graph->VS.shad_static = VS.shad_static.get();
    graph->VS.shad_skelly = VS.shad_skelly.get();
    graph->VS.shad_static_punch = VS.shad_static_punch.get();
    graph->VS.shad_skelly_punch = VS.shad_skelly_punch.get();
    graph->FS.modl_write = FS.modl_write.get();
    graph->FS.shad_punch = FS.shad_punch.get();
    graph->FS.shds_ambient = FS.shds_ambient.get();
    graph->FS.shds_skylight = FS.shds_skylight.get();
    graph->FS.shds_spotlight = FS.shds_spotlight.get();
    graph->FS.shds_pointlight = FS.shds_pointlight.get();
    graph->pipeline = pipeline.get();
}

void SceneMain::resize(uvec2 _size) {
    INFO.fullSize = _size;
    INFO.halfSize = _size / 2u;
    camera->size = vec2(_size);
    vec2 fullPixSize = 1.f / vec2(INFO.fullSize);
    vec2 halfPixSize = 1.f / vec2(INFO.halfSize);

    TX.defrNorm->resize(INFO.fullSize);
    TX.defrSurf->resize(INFO.fullSize);
    TX.defrDiff->resize(INFO.fullSize);
    TX.defrSpec->resize(INFO.fullSize);
    TX.defrDepth->resize(INFO.fullSize);
    TX.ssaoGreyA->resize(INFO.halfSize);
    TX.ssaoGreyB->resize(INFO.halfSize);
    TX.hdrColour->resize(INFO.fullSize);
    TX.basicColour->resize(INFO.fullSize);
    TX.basicDepth->resize(INFO.fullSize);

    FS.prty_ssao_ssao_low->set_vec<vec2>("pixSize", halfPixSize);
    FS.prty_ssao_ssao_high->set_vec<vec2>("pixSize", halfPixSize);
    FS.prty_ssao_blur->set_vec<vec2>("pixSize", halfPixSize);
    FS.prty_fxaa_fxaa_low->set_vec<vec2>("pixSize", fullPixSize);
    FS.prty_fxaa_fxaa_high->set_vec<vec2>("pixSize", fullPixSize);
}

void SceneMain::update() {
    using KB = sf::Keyboard;
    posCrnt = posNext;

    if (KB::isKeyPressed(KB::PageUp)) posNext.z += 0.05f;
    if (KB::isKeyPressed(KB::PageDown)) posNext.z -= 0.05f;

    if (KB::isKeyPressed(KB::Right) && !KB::isKeyPressed(KB::Left))
        posNext += glm::rotateZ(vec3(0.08f, 0.f, 0.f), rotZ);
    else if (KB::isKeyPressed(KB::Left) && !KB::isKeyPressed(KB::Right))
        posNext += glm::rotateZ(vec3(-0.08f, 0.f, 0.f), rotZ);

    if (KB::isKeyPressed(KB::Up) && !KB::isKeyPressed(KB::Down))
        posNext += glm::rotateZ(vec3(0.f, 0.08f, 0.f), rotZ);
    else if (KB::isKeyPressed(KB::Down) && !KB::isKeyPressed(KB::Up))
        posNext += glm::rotateZ(vec3(0.f, -0.08f, 0.f), rotZ);

    world->tick();
    graph->update();
}

void SceneMain::render(float _ft) {
    if (app.settings.check_update("SceneGame"))
        update_settings();

    const double dt = 1.0 / 24.0;
    camera->pos = glm::mix(posCrnt, posNext, accum / dt);

    if (app.settings.crnt<bool>("mouseFocus")) {
        vec2 mMove = app.mouse_relatify();
        rotZ = rotZ + mMove.x/600.f;
        rotX = glm::clamp(rotX + mMove.y/900.f, -1.1f, 1.1f);
        camera->dir = glm::rotateZ(glm::rotateX(vec3(0,1,0), rotX), rotZ);
    } camera->update();

    world->calc(accum);


    /// World Updates ///

        FS.shds_ambient->set_vec<vec3>("lightColour", {0.5,0.5,0.5});
        // spotlight stuff...


    /// Per-frame setup ///
    pipeline->bind();
    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    gl::Enable(gl::CULL_FACE);
    gl::CullFace(gl::BACK);


    /// Light Shadows ///
    graph->render_shadows_sky();
    graph->render_shadows_spot();
    graph->render_shadows_point();
    gl::FrontFace(gl::CCW);


    /// Liquids and submerged areas ///
    // Redo from scratch


    /// Main Rendering ///
    FB.defr->use();
    gl::Viewport(0, 0, INFO.fullSize.x, INFO.fullSize.y);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
    graph->render_models(); gl::FrontFace(gl::CCW);


    /// Set up Screen Space ///
    pipeline->use_shader(*VS.gnrc_quad);
    gl::Disable(gl::DEPTH_TEST);

    TX.defrDiff->bind(gl::TEXTURE0);
    TX.defrNorm->bind(gl::TEXTURE1);
    TX.defrSurf->bind(gl::TEXTURE2);
    TX.defrSpec->bind(gl::TEXTURE3);
    TX.ssaoGreyA->bind(gl::TEXTURE4);
    TX.defrDepth->bind(gl::TEXTURE5);


    /// SSAO ///
    if (INFO.ssaoQuality > 0) {
        if (INFO.ssaoQuality == 1) pipeline->use_shader(*FS.prty_ssao_ssao_low);
        if (INFO.ssaoQuality == 2) pipeline->use_shader(*FS.prty_ssao_ssao_high);

        FB.ssaoA->use();
        gl::Viewport(0, 0, INFO.halfSize.x, INFO.halfSize.y);
        sq::draw_screen_quad();

        FB.ssaoB->use();
        pipeline->use_shader(*FS.prty_ssao_blur);
        sq::draw_screen_quad();
        TX.ssaoGreyB->bind(gl::TEXTURE4);
    }

    FB.hdr->use();
    gl::Viewport(0, 0, INFO.fullSize.x, INFO.fullSize.y);
    gl::Clear(gl::COLOR_BUFFER_BIT);
    gl::BlendFunc(gl::ONE, gl::ONE);
    gl::Enable(gl::BLEND);

    pipeline->use_shader(*FS.shds_ambient);
    sq::draw_screen_quad();


    graph->render_lights_sky();
    graph->render_lights_spot();
    graph->render_lights_point();


    gl::Disable(gl::BLEND);
    TX.hdrColour->bind(gl::TEXTURE0);
    pipeline->use_shader(*FS.gnrc_lumalpha);
    sq::draw_screen_quad();

    FB.basic->use();
    pipeline->use_shader(*FS.gnrc_tonemap);
    sq::draw_screen_quad();

    /// FXAA ///
    TX.basicColour->bind(gl::TEXTURE0);
    if (INFO.fxaaQuality > 0) {
        pipeline->use_shader(*FS.gnrc_lumalpha);
        sq::draw_screen_quad();
        if (INFO.fxaaQuality == 1) pipeline->use_shader(*FS.prty_fxaa_fxaa_low);
        if (INFO.fxaaQuality == 2) pipeline->use_shader(*FS.prty_fxaa_fxaa_high);
    } else pipeline->use_shader(*FS.gnrc_passthru);

    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    sq::draw_screen_quad();

    if (INFO.vignetting) {
        gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
        gl::Enable(gl::BLEND);
        pipeline->use_shader(*FS.prty_vignette);
        sq::draw_screen_quad();
        gl::Disable(gl::BLEND);
    }


//    pipeline->use_shader(*VS.gnrc_quad);
//    pipeline->use_shader(*FS.gnrc_passthru_layer);
//    graph->lightSkyList.front().lock()->sky->tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::NONE);
//    graph->lightSkyList.front().lock()->sky->tex->bind(gl::TEXTURE0);
//    sq::draw_grid4x4_quad<sq::Grid4x4::AD>();
//    graph->lightSkyList.front().lock()->sky->tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);



    gl::BindVertexArray(0);
}


void SceneMain::update_settings() {
    INFO.viewDistance = app.settings.crnt<float>("viewDistance");
    INFO.farDistance = app.settings.crnt<float>("farDistance");
    INFO.shadQuality = app.settings.crnt<int>("shadQuality");
    INFO.shadFilter = app.settings.crnt<int>("shadFilter");
    INFO.ssaoQuality = app.settings.crnt<int>("ssaoQuality");
    INFO.fxaaQuality = app.settings.crnt<int>("fxaaQuality");
    INFO.vignetting = app.settings.crnt<bool>("vignetting");

    camera->range.y = INFO.viewDistance;

    if (INFO.ssaoQuality == 0) {
        FB.ssaoA->use();
        gl::Viewport(0, 0, INFO.halfSize.x, INFO.halfSize.y);
        gl::ClearColor(1.f, 1.f, 1.f, 1.f);
        gl::Clear(gl::COLOR_BUFFER_BIT);
        gl::ClearColor(0.f, 0.f, 0.f, 0.f);
    }

    graph->update_settings();

    FS.shds_skylight->set_sca<int>("shadQuality", INFO.shadQuality);
    FS.shds_skylight->set_sca<int>("shadFilter", INFO.shadFilter);
    FS.shds_spotlight->set_sca<int>("shadQuality", INFO.shadQuality);
    FS.shds_spotlight->set_sca<int>("shadFilter", INFO.shadFilter);
    FS.shds_pointlight->set_sca<int>("shadQuality", INFO.shadQuality);
    FS.shds_pointlight->set_sca<int>("shadFilter", INFO.shadFilter);
}
