#include <list>
#include <map>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/application.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/misc.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/cameras.hpp>

#include "../rndr/graph.hpp"
#include "../wcoe/world.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/spotlight.hpp"
#include "../wcoe/obj/pointlight.hpp"
#include "camera.hpp"
#include "scenemain.hpp"

using namespace sqt;

SceneMain::SceneMain(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 24;

    pipeline.reset(new sq::Pipeline());
    camera.reset(new MainCamera());
    world.reset(new wcoe::World());
    graph.reset(new rndr::Graph());
    world->camera = camera.get();
    world->skybox.camera = camera.get();
    world->ambient.camera = camera.get();
    world->skylight.camera = camera.get();
    world->settings = &app.settings;
    graph->camera = camera.get();
    graph->world = world.get();
    graph->pipeline = pipeline.get();
    graph->settings = &app.settings;

    app.cs->add(chai::var(camera), "camera");
    app.cs->add(chai::var(world), "world");
    app.cs->add(chai::var(graph), "graph");

    camera->pos = {0.f, -1.f, 3.f};
    camera->dir = {0.7, 0.2, -0.1};
    camera->range = {0.2f, 120.f};
    camera->size = {16.f, 10.f};
    camera->fov = 1.f;
    camera->binding = 0u;
    posNext = camera->pos;
    camera->update();

    world->add_cell("GLOBAL", {0,0,0});
    world->enable_cell("GLOBAL");


    /// Player Model


    /// Add Settings
    app.settings.add<float>("viewDist", 120.f);
    app.settings.add<int>("shadQlty", 2); // 0=1024, 1=2048, 2=4096
    app.settings.add<int>("shadFltr", 2); // 0=Off, 1=Minimal, 2=Full
    app.settings.add<int>("ssaoQlty", 2); // 0=Off, 1=Low, 2=High
    app.settings.add<int>("hdrbQlty", 2); // 0=Off, 1=Low, 2=High
    app.settings.add<int>("fxaaQlty", 2); // 0=Off, 1=Low, 2=High
    app.settings.add<bool>("vignette", true);
    app.settings.add<bool>("mouseFocus", false);

    /// Create Framebuffers
    FB.defr.reset(new sq::Framebuffer());
    FB.ssaoA.reset(new sq::Framebuffer());
    FB.ssaoB.reset(new sq::Framebuffer());
    FB.bloomA.reset(new sq::Framebuffer());
    FB.bloomB.reset(new sq::Framebuffer());
    FB.hdr.reset(new sq::Framebuffer());
    FB.basic.reset(new sq::Framebuffer());

    /// Create Textures
    TX.defrNorm.reset(new sq::Texture2D());
    TX.defrSurf.reset(new sq::Texture2D());
    TX.defrDiff.reset(new sq::Texture2D());
    TX.defrSpec.reset(new sq::Texture2D());
    TX.defrDepth.reset(new sq::Texture2D());
    TX.ssaoA.reset(new sq::Texture2D());
    TX.ssaoB.reset(new sq::Texture2D());
    TX.bloomA.reset(new sq::Texture2D());
    TX.bloomB.reset(new sq::Texture2D());
    TX.hdrColour.reset(new sq::Texture2D());
    TX.basicColour.reset(new sq::Texture2D());
    TX.basicDepth.reset(new sq::Texture2D());

    /// Setup Textures
    TX.defrDiff->create(gl::RGBA, gl::RGBA8, 4);
    TX.defrNorm->create(gl::RGB, gl::RGB16, 3);
    TX.defrSurf->create(gl::RGB, gl::RGB16, 3);
    TX.defrSpec->create(gl::RGBA, gl::RGBA8, 4);
    TX.defrDepth->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1);
    TX.ssaoA->create(gl::RED, gl::R8, 1);
    TX.ssaoB->create(gl::RED, gl::R8, 1);
    TX.bloomA->create(gl::RGB, gl::RGB8, 3);
    TX.bloomB->create(gl::RGB, gl::RGB8, 3);
    TX.hdrColour->create(gl::RGBA, gl::RGBA16F, 4);
    TX.basicColour->create(gl::RGBA, gl::RGBA8, 4);
    TX.basicDepth->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1);
    TX.defrDiff->set_preset(sq::Texture2D::L_C());
    TX.defrNorm->set_preset(sq::Texture2D::L_C());
    TX.defrSurf->set_preset(sq::Texture2D::L_C());
    TX.defrSpec->set_preset(sq::Texture2D::L_C());
    TX.defrDepth->set_preset(sq::Texture2D::L_C());
    TX.ssaoA->set_preset(sq::Texture2D::L_C());
    TX.ssaoB->set_preset(sq::Texture2D::L_C());
    TX.bloomA->set_preset(sq::Texture2D::L_C());
    TX.bloomB->set_preset(sq::Texture2D::L_C());
    TX.hdrColour->set_preset(sq::Texture2D::L_C());
    TX.basicColour->set_preset(sq::Texture2D::L_C());
    TX.basicDepth->set_preset(sq::Texture2D::L_C());

    /// Setup Framebuffers
    FB.defr->attach(gl::COLOR_ATTACHMENT0, *TX.defrDiff);
    FB.defr->attach(gl::COLOR_ATTACHMENT1, *TX.defrNorm);
    FB.defr->attach(gl::COLOR_ATTACHMENT2, *TX.defrSurf);
    FB.defr->attach(gl::COLOR_ATTACHMENT3, *TX.defrSpec);
    FB.defr->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.defrDepth);
    FB.ssaoA->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoA);
    FB.ssaoB->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoB);
    FB.bloomA->attach(gl::COLOR_ATTACHMENT0, *TX.bloomA);
    FB.bloomB->attach(gl::COLOR_ATTACHMENT0, *TX.bloomB);
    FB.hdr->attach(gl::COLOR_ATTACHMENT0, *TX.hdrColour);
    FB.hdr->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.defrDepth);
    FB.basic->attach(gl::COLOR_ATTACHMENT0, *TX.basicColour);
    FB.basic->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.basicDepth);

    /// Import GLSL Headers
    app.preproc.import_header("headers/blocks/camera");
    app.preproc.import_header("headers/blocks/skybox");
    app.preproc.import_header("headers/blocks/ambient");
    app.preproc.import_header("headers/blocks/skylight");
    app.preproc.import_header("headers/blocks/spotlight");
    app.preproc.import_header("headers/blocks/pointlight");
    app.preproc.import_header("headers/uniform_disks");
    app.preproc.import_header("headers/shadow/sample_sky");
    app.preproc.import_header("headers/shadow/sample_spot");
    app.preproc.import_header("headers/shadow/sample_point");

    /// Create Shaders
    VS.gnrc_screen.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.gnrc_fillwith.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_passthru.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_lumalpha.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.modl_static.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.modl_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.modl_write.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.shds_skybox.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.shds_skybox.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_ambient.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_skylight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.shds_spotstncl.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shds_pointstncl.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.shds_spot_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spot_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spot_spec.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spot_both.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_spec.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_both.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.shad_static.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.shad_punch.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_ssao_ssao.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_ssao_blur.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_blmhigh.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_blmblur.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_tonemap.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_fxaa_fxaa.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_vignette.reset(new sq::Shader(gl::FRAGMENT_SHADER));

    /// Add Uniforms to Shaders
    VS.modl_static->add_uniform("modelMat"); // mat4
    VS.modl_static->add_uniform("normMat"); // mat3
    VS.modl_skelly->add_uniform("modelMat"); // mat4
    VS.modl_skelly->add_uniform("normMat"); // mat3
    VS.modl_skelly->add_uniform("skelQuat", 40); // vec4
    VS.modl_skelly->add_uniform("skelOffs", 40); // vec3
    VS.shad_static->add_uniform("matrix"); // mat4
    VS.shad_skelly->add_uniform("matrix"); // mat4
    VS.shad_skelly->add_uniform("skelQuat", 40); // vec4
    VS.shad_skelly->add_uniform("skelOffs", 40); // vec3
    FS.modl_write->add_uniform("diff_norm_spec"); // int

    /// Load Shaders
    VS.gnrc_screen->load(app.preproc("generic/screen_vs"));
    FS.gnrc_fillwith->load(app.preproc("generic/fillwith_fs"));
    FS.gnrc_lumalpha->load(app.preproc("generic/lumalpha_fs"));
    FS.gnrc_passthru->load(app.preproc("generic/passthru_fs"));
    VS.modl_static->load(app.preproc("models/static_vs"));
    VS.modl_skelly->load(app.preproc("models/skelly_vs"));
    FS.modl_write->load(app.preproc("models/write_fs"));
    VS.shds_skybox->load(app.preproc("shades/skybox_vs"));
    FS.shds_skybox->load(app.preproc("shades/skybox_fs"));
    VS.shds_spotstncl->load(app.preproc("shades/spotstncl_vs"));
    VS.shds_pointstncl->load(app.preproc("shades/pointstncl_vs"));
    VS.shad_static->load(app.preproc("shadows/static_vs"));
    VS.shad_skelly->load(app.preproc("shadows/skelly_vs"));
    FS.shad_punch->load(app.preproc("shadows/punch_fs"));
    FS.prty_hdr_blmhigh->load(app.preproc("pretty/hdr/blmhigh_fs"));
    FS.prty_vignette->load(app.preproc("pretty/vignette/vignette_fs"));

    /// Set Graph Shaders
    graph->VS.gnrc_screen = VS.gnrc_screen.get();
    graph->FS.gnrc_fillwith = FS.gnrc_fillwith.get();
    graph->FS.gnrc_lumalpha = FS.gnrc_lumalpha.get();
    graph->FS.gnrc_passthru = FS.gnrc_passthru.get();
    graph->VS.modl_static = VS.modl_static.get();
    graph->VS.modl_skelly = VS.modl_skelly.get();
    graph->VS.shds_skybox = VS.shds_skybox.get();
    graph->VS.shad_static = VS.shad_static.get();
    graph->VS.shad_skelly = VS.shad_skelly.get();
    graph->FS.modl_write = FS.modl_write.get();
    graph->FS.shad_punch = FS.shad_punch.get();
    graph->FS.shds_skybox = FS.shds_skybox.get();
    graph->FS.shds_ambient = FS.shds_ambient.get();
    graph->FS.shds_skylight = FS.shds_skylight.get();
    graph->VS.shds_spotstncl = VS.shds_spotstncl.get();
    graph->VS.shds_pointstncl = VS.shds_pointstncl.get();
    graph->FS.shds_spot_none = FS.shds_spot_none.get();
    graph->FS.shds_spot_shad = FS.shds_spot_shad.get();
    graph->FS.shds_spot_spec = FS.shds_spot_spec.get();
    graph->FS.shds_spot_both = FS.shds_spot_both.get();
    graph->FS.shds_point_none = FS.shds_point_none.get();
    graph->FS.shds_point_shad = FS.shds_point_shad.get();
    graph->FS.shds_point_spec = FS.shds_point_spec.get();
    graph->FS.shds_point_both = FS.shds_point_both.get();
}

void SceneMain::resize(uvec2 _size) {
    INFO.fullSize = _size;
    INFO.halfSize = _size / 2u;
    INFO.qterSize = _size / 4u;
    INFO.fPixSize = 1.f / vec2(INFO.fullSize);
    INFO.hPixSize = 1.f / vec2(INFO.halfSize);
    INFO.qPixSize = 1.f / vec2(INFO.qterSize);
    camera->size = vec2(_size);

    TX.defrNorm->resize(INFO.fullSize);
    TX.defrSurf->resize(INFO.fullSize);
    TX.defrDiff->resize(INFO.fullSize);
    TX.defrSpec->resize(INFO.fullSize);
    TX.defrDepth->resize(INFO.fullSize);
    TX.ssaoA->resize(INFO.halfSize);
    TX.ssaoB->resize(INFO.halfSize);
    TX.bloomA->resize(INFO.qterSize);
    TX.bloomB->resize(INFO.qterSize);
    TX.hdrColour->resize(INFO.fullSize);
    TX.basicColour->resize(INFO.fullSize);
    TX.basicDepth->resize(INFO.fullSize);

    reload_shaders();
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


    /// Per-frame setup
    pipeline->bind();
    pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);
    gl::Enable(gl::CULL_FACE);
    gl::CullFace(gl::BACK);
    gl::DepthMask(true);


    /// Light Shadows
    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    graph->render_shadows_sky();
    graph->render_shadows_spot();
    graph->render_shadows_point();
    gl::FrontFace(gl::CCW);


    /// Render Models into G-Buffer
    gl::Viewport(0, 0, INFO.fullSize.x, INFO.fullSize.y);
    gl::Enable(gl::STENCIL_TEST); gl::StencilMask(0xff); FB.defr->use();
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
    gl::StencilFunc(gl::ALWAYS, 0x01, 0xff);
    graph->render_models(); gl::FrontFace(gl::CCW);
    gl::Disable(gl::STENCIL_TEST);


    /// Set up Screen Space
    gl::Disable(gl::DEPTH_TEST);
    gl::Disable(gl::CULL_FACE);
    gl::BlendFunc(gl::ONE, gl::ONE);
    gl::DepthMask(false);
    TX.defrDiff->bind(gl::TEXTURE0);
    TX.defrNorm->bind(gl::TEXTURE1);
    TX.defrSurf->bind(gl::TEXTURE2);
    TX.defrSpec->bind(gl::TEXTURE3);
    TX.defrDepth->bind(gl::TEXTURE4);


    /// Render SSAO Texture
    if (INFO.ssaoQlty > 0) {
        pipeline->use_shader(*VS.gnrc_screen);
        pipeline->use_shader(*FS.prty_ssao_ssao);
        gl::Viewport(0, 0, INFO.halfSize.x, INFO.halfSize.y);
        FB.ssaoA->use(); sq::draw_screen_quad(); TX.ssaoA->bind(gl::TEXTURE5);
        pipeline->use_shader(*FS.prty_ssao_blur);
        FB.ssaoB->use(); sq::draw_screen_quad(); TX.ssaoB->bind();
        FB.ssaoA->use(); sq::draw_screen_quad(); TX.ssaoA->bind();
        FB.ssaoB->use(); sq::draw_screen_quad(); TX.ssaoB->bind();
    }


    /// SkyBox and Lighting
    gl::Viewport(0, 0, INFO.fullSize.x, INFO.fullSize.y);
    FB.hdr->use(); gl::Clear(gl::COLOR_BUFFER_BIT);
    gl::Enable(gl::STENCIL_TEST); gl::StencilMask(0x00);
    gl::StencilFunc(gl::EQUAL, 0x00, 0xff); graph->render_skybox();
    gl::StencilFunc(gl::EQUAL, 0x01, 0xff); graph->render_ambient();
    gl::Enable(gl::BLEND); graph->render_skylight();
    graph->render_spotlights(); graph->render_pointlights();
    gl::Disable(gl::BLEND); gl::Disable(gl::STENCIL_TEST);


    /// Write HDR Luma to Alpha
    TX.hdrColour->bind(gl::TEXTURE0);
    pipeline->use_shader(*FS.gnrc_lumalpha);
    gl::Viewport(0, 0, INFO.fullSize.x, INFO.fullSize.y);
    sq::draw_screen_quad();


    /// Bloom
    if (INFO.hdrbQlty > 0) {
        pipeline->use_shader(*FS.prty_hdr_blmhigh);
        gl::Viewport(0, 0, INFO.qterSize.x, INFO.qterSize.y);
        FB.bloomA->use(); sq::draw_screen_quad(); TX.bloomA->bind(gl::TEXTURE1);
        pipeline->use_shader(*FS.prty_hdr_blmblur);
        FB.bloomB->use(); sq::draw_screen_quad(); TX.bloomB->bind();
        FB.bloomA->use(); sq::draw_screen_quad(); TX.bloomA->bind();
        FB.bloomB->use(); sq::draw_screen_quad(); TX.bloomB->bind();
        if (INFO.hdrbQlty == 2) {
            FB.bloomA->use(); sq::draw_screen_quad(); TX.bloomA->bind();
            FB.bloomB->use(); sq::draw_screen_quad(); TX.bloomB->bind();
            FB.bloomA->use(); sq::draw_screen_quad(); TX.bloomA->bind();
            FB.bloomB->use(); sq::draw_screen_quad(); TX.bloomB->bind();
            FB.bloomA->use(); sq::draw_screen_quad(); TX.bloomA->bind();
            FB.bloomB->use(); sq::draw_screen_quad(); TX.bloomB->bind();
        }
    }

    /// HDR Tonemapping
    pipeline->use_shader(*FS.prty_hdr_tonemap);
    gl::Viewport(0, 0, INFO.fullSize.x, INFO.fullSize.y);
    FB.basic->use(); sq::draw_screen_quad();


    /// Antialiasing and Write to Screen
    TX.basicColour->bind(gl::TEXTURE0);
    if (INFO.fxaaQlty > 0) {
        pipeline->use_shader(*FS.gnrc_lumalpha);
        sq::draw_screen_quad();
        pipeline->use_shader(*FS.prty_fxaa_fxaa);
    } else pipeline->use_shader(*FS.gnrc_passthru);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    sq::draw_screen_quad();


    /// Vignetting Effect
    if (INFO.vignette == true) {
        gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
        gl::Enable(gl::BLEND);
        pipeline->use_shader(*FS.prty_vignette);
        sq::draw_screen_quad();
        gl::Disable(gl::BLEND);
    }

    gl::BindProgramPipeline(0);
    gl::BindVertexArray(0);
}


void SceneMain::update_settings() {
    INFO.viewDist = app.settings.crnt<float>("viewDist");
    INFO.shadQlty = app.settings.crnt<int>("shadQlty");
    INFO.shadFltr = app.settings.crnt<int>("shadFltr");
    INFO.ssaoQlty = app.settings.crnt<int>("ssaoQlty");
    INFO.hdrbQlty = app.settings.crnt<int>("hdrbQlty");
    INFO.fxaaQlty = app.settings.crnt<int>("fxaaQlty");
    INFO.vignette = app.settings.crnt<bool>("vignette");

    camera->range.y = INFO.viewDist;

    graph->update_settings();
    reload_shaders();
}


void SceneMain::reload_shaders() {
    /// Shadows
    string defines = "#define SHADQLTY " + std::to_string(INFO.shadQlty) + "\n"
                     "#define SHADFLTR " + std::to_string(INFO.shadFltr);
    string definesShad = defines + "\n#define SHADOW";
    string definesSpec = defines + "\n#define SPECULAR";
    string definesBoth = defines + "\n#define SHADOW\n#define SPECULAR";
    FS.shds_skylight->load(app.preproc("shades/skylight_fs", defines));
    FS.shds_spot_none->load(app.preproc("shades/spotlight_fs", defines));
    FS.shds_spot_shad->load(app.preproc("shades/spotlight_fs", definesShad));
    FS.shds_spot_spec->load(app.preproc("shades/spotlight_fs", definesSpec));
    FS.shds_spot_both->load(app.preproc("shades/spotlight_fs", definesBoth));
    FS.shds_point_none->load(app.preproc("shades/pointlight_fs", defines));
    FS.shds_point_shad->load(app.preproc("shades/pointlight_fs", definesShad));
    FS.shds_point_spec->load(app.preproc("shades/pointlight_fs", definesSpec));
    FS.shds_point_both->load(app.preproc("shades/pointlight_fs", definesBoth));

    /// HDR
    if (INFO.hdrbQlty == 0) FS.prty_hdr_tonemap->load(app.preproc("pretty/hdr/tonemap_fs"));
    else FS.prty_hdr_tonemap->load(app.preproc("pretty/hdr/tonemap_fs", "#define HDRB"));
    if (INFO.hdrbQlty > 0) {
        defines = "#define PIXSIZE " + glm::to_string(INFO.qPixSize*2.5f);
        FS.prty_hdr_blmblur->load(app.preproc("pretty/hdr/blmblur_fs", defines)); }

    /// FXAA
    if (INFO.fxaaQlty == 1) {
        defines = "#define PIXSIZE " + glm::to_string(INFO.fPixSize);
        FS.prty_fxaa_fxaa->load(app.preproc("pretty/fxaa/fxaa_fs", defines)); }
    if (INFO.fxaaQlty == 2) {
        defines = "#define HIGH\n#define PIXSIZE " + glm::to_string(INFO.fPixSize);
        FS.prty_fxaa_fxaa->load(app.preproc("pretty/fxaa/fxaa_fs", defines)); }

    /// SSAO
    if (INFO.ssaoQlty == 0) FS.shds_ambient->load(app.preproc("shades/ambient_fs"));
    else FS.shds_ambient->load(app.preproc("shades/ambient_fs", "#define SSAO"));
    if (INFO.ssaoQlty == 1) {
        defines = "#define PIXSIZE " + glm::to_string(INFO.hPixSize);
        FS.prty_ssao_ssao->load(app.preproc("pretty/ssao/ssao_fs", defines));
        FS.prty_ssao_blur->load(app.preproc("pretty/ssao/blur_fs", defines)); }
    if (INFO.ssaoQlty == 2) {
        defines = "#define HIGH\n#define PIXSIZE " + glm::to_string(INFO.hPixSize);
        FS.prty_ssao_ssao->load(app.preproc("pretty/ssao/ssao_fs", defines));
        FS.prty_ssao_blur->load(app.preproc("pretty/ssao/blur_fs", defines)); }
}
