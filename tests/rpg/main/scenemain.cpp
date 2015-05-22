#include <list>
#include <map>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/application.hpp>
#include <sqee/app/settings.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/gl/preprocessor.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/misc.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/cameras.hpp>

#include "../rndr/graph.hpp"
#include "../wcoe/world.hpp"
#include "../wcoe/cell.hpp"
#include "camera.hpp"
#include "scenemain.hpp"

using namespace sqt;

#define CLIP_ON gl::Enable(gl::CLIP_DISTANCE0)
#define CLIP_OFF gl::Disable(gl::CLIP_DISTANCE0)
#define BLEND_ALPHA gl::Enable(gl::BLEND), gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA)
#define BLEND_ONEONE gl::Enable(gl::BLEND), gl::BlendFunc(gl::ONE, gl::ONE)
#define BLEND_OFF gl::Disable(gl::BLEND)
#define CULLFACE_BACK gl::Enable(gl::CULL_FACE), gl::CullFace(gl::BACK)
#define CULLFACE_FRONT gl::Enable(gl::CULL_FACE), gl::CullFace(gl::FRONT)
#define CULLFACE_OFF gl::Disable(gl::CULL_FACE)
#define DEPTHTEST_RO gl::Enable(gl::DEPTH_TEST), gl::DepthMask(0)
#define DEPTHTEST_RW gl::Enable(gl::DEPTH_TEST), gl::DepthMask(1)
#define DEPTHTEST_OFF gl::Disable(gl::DEPTH_TEST)
#define STENCILTEST_ON gl::Enable(gl::STENCIL_TEST)
#define STENCILTEST_OFF gl::Disable(gl::STENCIL_TEST)
#define CLEAR_COLOR gl::Clear(gl::COLOR_BUFFER_BIT)
#define CLEAR_DEPTH gl::Clear(gl::DEPTH_BUFFER_BIT)
#define CLEAR_STENC gl::Clear(gl::STENCIL_BUFFER_BIT)
#define CLEAR_COLOR_DEPTH gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT)
#define CLEAR_COLOR_STENC gl::Clear(gl::COLOR_BUFFER_BIT | gl::STENCIL_BUFFER_BIT)
#define CLEAR_DEPTH_STENC gl::Clear(gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT)
#define CLEAR_COLOR_DEPTH_STENC gl::Clear(gl::COLOR_BUFFER_BIT|gl::DEPTH_BUFFER_BIT|gl::STENCIL_BUFFER_BIT)
#define VIEWPORT_FULL gl::Viewport(0, 0, INFO.fullSize.x, INFO.fullSize.y)
#define VIEWPORT_HALF gl::Viewport(0, 0, INFO.halfSize.x, INFO.halfSize.y)
#define VIEWPORT_QTER gl::Viewport(0, 0, INFO.qterSize.x, INFO.qterSize.y)


SceneMain::SceneMain(sq::Application* const _app) : sq::Scene(_app) {
    tickRate = 24u;

    camera.reset(new MainCamera());
    pipeline.reset(new sq::Pipeline());
    world.reset(new wcoe::World(camera.get(), settings));
    graph.reset(new rndr::Graph(camera.get(), settings));
    graph->pipeline = pipeline.get();
    graph->world = world.get();

    appBase->cs->add_global(chai::var(camera.get()), "camera");
    appBase->cs->add_global(chai::var(world.get()), "world");
    appBase->cs->add_global(chai::var(graph.get()), "graph");

    camera->pos = {0.f, -1.f, 3.f};
    camera->dir = {0.7, 0.2, -0.1};
    camera->range = {0.2f, 120.f};
    camera->size = {16.f, 10.f};
    camera->fov = 1.f;
    camera->binding = 0u;
    posNext = camera->pos;
    camera->update();

//    world->add_cell("GLOBAL", {0,0,0})->DAT_enabled = true;

    /// Player Model


    /// Add Settings
    settings->add<float>("viewDist", 120.f);
    settings->add<int>("shadQlty", 2); // 0=1024, 1=2048, 2=4096
    settings->add<int>("shadFltr", 2); // 0=Off, 1=Minimal, 2=Full
    settings->add<int>("ssaoMode", 2); // 0=Off, 1=Low, 2=High
    settings->add<int>("hdrbMode", 2); // 0=Off, 1=Low, 2=High
    settings->add<int>("fxaaMode", 2); // 0=Off, 1=Low, 2=High
    settings->add<int>("vignMode", 2); // 0=Off, 1=Low, 2=High
    settings->add<bool>("mouseFocus", false);

    /// Create Framebuffers
    FB.defr.reset(new sq::Framebuffer());
    FB.hdr.reset(new sq::Framebuffer());
    FB.basic.reset(new sq::Framebuffer());
    FB.ssaoA.reset(new sq::Framebuffer());
    FB.ssaoB.reset(new sq::Framebuffer());
    FB.bloomA.reset(new sq::Framebuffer());
    FB.bloomB.reset(new sq::Framebuffer());

    /// Create Textures
    TX.depth.reset(new sq::Texture2D());
    TX.diff.reset(new sq::Texture2D());
    TX.surf.reset(new sq::Texture2D());
    TX.norm.reset(new sq::Texture2D());
    TX.spec.reset(new sq::Texture2D());
    TX.hdr.reset(new sq::Texture2D());
    TX.basic.reset(new sq::Texture2D());
    TX.ssaoA.reset(new sq::Texture2D());
    TX.ssaoB.reset(new sq::Texture2D());
    TX.bloomA.reset(new sq::Texture2D());
    TX.bloomB.reset(new sq::Texture2D());

    /// Setup Textures
    TX.depth->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1);
    TX.diff->create(gl::RGB, gl::RGB8, 3);
    TX.surf->create(gl::RGB, gl::RGB12, 3);
    TX.norm->create(gl::RGB, gl::RGB12, 3);
    TX.spec->create(gl::RGB, gl::RGB8, 3);
    TX.hdr->create(gl::RGBA, gl::RGBA16F, 4);
    TX.basic->create(gl::RGBA, gl::RGBA8, 4);
    TX.ssaoA->create(gl::RED, gl::R8, 1);
    TX.ssaoB->create(gl::RED, gl::R8, 1);
    TX.bloomA->create(gl::RGB, gl::RGB8, 3);
    TX.bloomB->create(gl::RGB, gl::RGB8, 3);
    TX.depth->set_preset(sq::Texture2D::L_C());
    TX.diff->set_preset(sq::Texture2D::L_C());
    TX.surf->set_preset(sq::Texture2D::L_C());
    TX.norm->set_preset(sq::Texture2D::L_C());
    TX.spec->set_preset(sq::Texture2D::L_C());
    TX.hdr->set_preset(sq::Texture2D::L_C());
    TX.basic->set_preset(sq::Texture2D::L_C());
    TX.ssaoA->set_preset(sq::Texture2D::L_C());
    TX.ssaoB->set_preset(sq::Texture2D::L_C());
    TX.bloomA->set_preset(sq::Texture2D::L_C());
    TX.bloomB->set_preset(sq::Texture2D::L_C());

    /// Setup Framebuffers
    FB.defr->attach(gl::COLOR_ATTACHMENT0, *TX.diff);
    FB.defr->attach(gl::COLOR_ATTACHMENT1, *TX.surf);
    FB.defr->attach(gl::COLOR_ATTACHMENT2, *TX.norm);
    FB.defr->attach(gl::COLOR_ATTACHMENT3, *TX.spec);
    FB.defr->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.depth);
    FB.hdr->attach(gl::COLOR_ATTACHMENT0, *TX.hdr);
    FB.hdr->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.depth);
    FB.basic->attach(gl::COLOR_ATTACHMENT0, *TX.basic);
    FB.ssaoA->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoA);
    FB.ssaoB->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoB);
    FB.bloomA->attach(gl::COLOR_ATTACHMENT0, *TX.bloomA);
    FB.bloomB->attach(gl::COLOR_ATTACHMENT0, *TX.bloomB);

    /// Import GLSL Headers
    preprocs->import_header("headers/blocks/camera");
    preprocs->import_header("headers/blocks/skybox");
    preprocs->import_header("headers/blocks/ambient");
    preprocs->import_header("headers/blocks/skylight");
    preprocs->import_header("headers/blocks/spotlight");
    preprocs->import_header("headers/blocks/pointlight");
    preprocs->import_header("headers/blocks/reflector");
    preprocs->import_header("headers/blocks/decal");
    preprocs->import_header("headers/uniform_disks");
    preprocs->import_header("headers/shadow/sample_sky");
    preprocs->import_header("headers/shadow/sample_spot");
    preprocs->import_header("headers/shadow/sample_point");

    /// Create Shaders
    VS.gnrc_screen.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.gnrc_fillwith.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_passthru.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_lumalpha.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.modl_static.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.modl_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.modl_stencil.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.modl_decal.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.modl_static_refl.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.modl_skelly_refl.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.modl_stencil_refl.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.modl_write.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.modl_decal.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.modl_write_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.shds_skybox.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.shds_skybox.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_ambient.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_skylight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spot_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spot_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spot_spec.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spot_both.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_spec.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_both.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.shds_skybox_refl.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.shds_ambient_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_skylight_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spot_none_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_spot_shad_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_none_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shds_point_shad_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.shds_reflector.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.shds_reflector.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    VS.shad_static.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.shad_punch.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_ssao_ssao.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_ssao_blur.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_highs.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_blurh.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_blurv.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_tones.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_fxaa_fxaa.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_vignette.reset(new sq::Shader(gl::FRAGMENT_SHADER));

    /// Add Uniforms to Shaders
    VS.modl_static->add_uniform("modelMat"); // mat4
    VS.modl_static->add_uniform("normMat"); // mat3
    VS.modl_skelly->add_uniform("modelMat"); // mat4
    VS.modl_skelly->add_uniform("normMat"); // mat3
    VS.modl_skelly->add_uniform("skelQuat", 40); // vec4
    VS.modl_skelly->add_uniform("skelOffs", 40); // vec3
    VS.modl_stencil->add_uniform("modelMat"); // mat4
    VS.modl_static_refl->add_uniform("modelMat"); // mat4
    VS.modl_static_refl->add_uniform("normMat"); // mat3
    VS.modl_skelly_refl->add_uniform("modelMat"); // mat4
    VS.modl_skelly_refl->add_uniform("normMat"); // mat3
    VS.modl_skelly_refl->add_uniform("skelQuat", 40); // vec4
    VS.modl_skelly_refl->add_uniform("skelOffs", 40); // vec3
    VS.modl_stencil_refl->add_uniform("modelMat"); // mat4
    VS.shad_static->add_uniform("matrix"); // mat4
    VS.shad_skelly->add_uniform("matrix"); // mat4
    VS.shad_skelly->add_uniform("skelQuat", 40); // vec4
    VS.shad_skelly->add_uniform("skelOffs", 40); // vec3
    FS.modl_write->add_uniform("d_n_s"); // ivec3
    FS.modl_write_refl->add_uniform("d_n_s"); // ivec3

    /// Load Shaders
    VS.gnrc_screen->load(preprocs->load("generic/screen_vs"));
    FS.gnrc_fillwith->load(preprocs->load("generic/fillwith_fs"));
    FS.gnrc_lumalpha->load(preprocs->load("generic/lumalpha_fs"));
    FS.gnrc_passthru->load(preprocs->load("generic/passthru_fs"));
    VS.modl_static->load(preprocs->load("models/static_vs"));
    VS.modl_skelly->load(preprocs->load("models/skelly_vs"));
    VS.modl_stencil->load(preprocs->load("models/stencil_vs"));
    VS.modl_decal->load(preprocs->load("models/decal_vs"));
    VS.modl_static_refl->load(preprocs->load("models/static_vs", "#define REFLECT"));
    VS.modl_skelly_refl->load(preprocs->load("models/skelly_vs", "#define REFLECT"));
    VS.modl_stencil_refl->load(preprocs->load("models/stencil_vs", "#define REFLECT"));
    FS.modl_write->load(preprocs->load("models/write_fs"));
    FS.modl_decal->load(preprocs->load("models/decal_fs"));
    FS.modl_write_refl->load(preprocs->load("models/write_fs", "#define REFLECT"));
    VS.shds_skybox->load(preprocs->load("shades/skybox_vs"));
    FS.shds_skybox->load(preprocs->load("shades/skybox_fs"));
    VS.shds_skybox_refl->load(preprocs->load("shades/skybox_vs", "#define REFLECT"));
    FS.shds_ambient_refl->load(preprocs->load("shades/ambient_fs", "#define REFLECT"));
    VS.shds_reflector->load(preprocs->load("shades/reflector_vs"));
    FS.shds_reflector->load(preprocs->load("shades/reflector_fs"));
    VS.shad_static->load(preprocs->load("shadows/static_vs"));
    VS.shad_skelly->load(preprocs->load("shadows/skelly_vs"));
    FS.shad_punch->load(preprocs->load("shadows/punch_fs"));
    FS.prty_hdr_highs->load(preprocs->load("pretty/hdr/highs_fs"));
    FS.prty_vignette->load(preprocs->load("pretty/vignette/vignette_fs"));

    /// Set Graph Vertex Shaders
    graph->VS.gnrc_screen = VS.gnrc_screen.get();
    graph->VS.modl_static = VS.modl_static.get();
    graph->VS.modl_skelly = VS.modl_skelly.get();
    graph->VS.modl_stencil = VS.modl_stencil.get();
    graph->VS.modl_decal = VS.modl_decal.get();
    graph->VS.modl_static_refl = VS.modl_static_refl.get();
    graph->VS.modl_skelly_refl = VS.modl_skelly_refl.get();
    graph->VS.modl_stencil_refl = VS.modl_stencil_refl.get();
    graph->VS.shad_static = VS.shad_static.get();
    graph->VS.shad_skelly = VS.shad_skelly.get();
    graph->VS.shds_skybox = VS.shds_skybox.get();
    graph->VS.shds_skybox_refl = VS.shds_skybox_refl.get();
    graph->VS.shds_reflector = VS.shds_reflector.get();

    /// Set Graph Fragment Shaders
    graph->FS.gnrc_fillwith = FS.gnrc_fillwith.get();
    graph->FS.gnrc_lumalpha = FS.gnrc_lumalpha.get();
    graph->FS.gnrc_passthru = FS.gnrc_passthru.get();
    graph->FS.modl_write = FS.modl_write.get();
    graph->FS.modl_decal = FS.modl_decal.get();
    graph->FS.modl_write_refl = FS.modl_write_refl.get();
    graph->FS.shad_punch = FS.shad_punch.get();
    graph->FS.shds_skybox = FS.shds_skybox.get();
    graph->FS.shds_ambient = FS.shds_ambient.get();
    graph->FS.shds_skylight = FS.shds_skylight.get();
    graph->FS.shds_spot_none = FS.shds_spot_none.get();
    graph->FS.shds_spot_shad = FS.shds_spot_shad.get();
    graph->FS.shds_spot_spec = FS.shds_spot_spec.get();
    graph->FS.shds_spot_both = FS.shds_spot_both.get();
    graph->FS.shds_point_none = FS.shds_point_none.get();
    graph->FS.shds_point_shad = FS.shds_point_shad.get();
    graph->FS.shds_point_spec = FS.shds_point_spec.get();
    graph->FS.shds_point_both = FS.shds_point_both.get();
    graph->FS.shds_ambient_refl = FS.shds_ambient_refl.get();
    graph->FS.shds_skylight_refl = FS.shds_skylight_refl.get();
    graph->FS.shds_spot_none_refl = FS.shds_spot_none_refl.get();
    graph->FS.shds_spot_shad_refl = FS.shds_spot_shad_refl.get();
    graph->FS.shds_point_none_refl = FS.shds_point_none_refl.get();
    graph->FS.shds_point_shad_refl = FS.shds_point_shad_refl.get();
    graph->FS.shds_reflector = FS.shds_reflector.get();

    /// Set Graph Textures
    graph->TX.depth = TX.depth.get();
    graph->TX.diff = TX.diff.get();
    graph->TX.surf = TX.surf.get();
    graph->TX.norm = TX.norm.get();
    graph->TX.spec = TX.spec.get();
    graph->TX.hdr = TX.hdr.get();

    /// Set Graph Framebuffers
    graph->FB.defr = FB.defr.get();
    graph->FB.hdr = FB.hdr.get();
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
    const double dt = 1.0 / 24.0;
    camera->pos = glm::mix(posCrnt, posNext, accum / dt);

    if (settings->crnt<bool>("mouseFocus")) {
        vec2 mMove = appBase->mouse_relatify();
        rotZ = rotZ + mMove.x/600.f;
        rotX = glm::clamp(rotX + mMove.y/900.f, -1.25f, 1.25f);
        camera->dir = glm::rotateZ(glm::rotateX(vec3(0,1,0), rotX), rotZ);
    } camera->update();

    world->calc(accum);


    /// Per-frame setup
    pipeline->bind();


    /// Light Shadows
    graph->render_shadows_sky_A();
    graph->render_shadows_sky_B();
    graph->render_shadows_spot();
    graph->render_shadows_point();


    /// Clear G-Buffer
    DEPTHTEST_RW; STENCILTEST_ON;
    gl::StencilMask(0b1111);
    FB.defr->use(); VIEWPORT_FULL;
    gl::ClearStencil(0b0100);
    CLEAR_COLOR_DEPTH_STENC;
    gl::ClearStencil(0b0000);


    /// Render Models into G-Buffer
    graph->render_mstatics(false);
    graph->render_mskellys(false);
    graph->render_reflects(false);
    graph->render_decals();


    /// Set up Screen Space
    TX.diff->bind(gl::TEXTURE0);
    TX.surf->bind(gl::TEXTURE1);
    TX.norm->bind(gl::TEXTURE2);
    TX.spec->bind(gl::TEXTURE3);
    TX.depth->bind(gl::TEXTURE4);


    /// Render SSAO Texture
    if (INFO.ssaoEnable == true) {
        BLEND_OFF; CULLFACE_OFF; DEPTHTEST_OFF; STENCILTEST_OFF;
        pipeline->use_shader(*VS.gnrc_screen);
        pipeline->use_shader(*FS.prty_ssao_ssao);
        FB.ssaoA->use(); VIEWPORT_HALF;
        sq::draw_screen_quad(); TX.ssaoA->bind(gl::TEXTURE5);
        pipeline->use_shader(*FS.prty_ssao_blur);
        FB.ssaoB->use(); sq::draw_screen_quad(); TX.ssaoB->bind();
        FB.ssaoA->use(); sq::draw_screen_quad(); TX.ssaoA->bind();
        FB.ssaoB->use(); sq::draw_screen_quad(); TX.ssaoB->bind();
    }


    /// Clear HDR Framebuffer
    CLIP_OFF; CULLFACE_OFF; DEPTHTEST_OFF;
    FB.hdr->use(); VIEWPORT_FULL; CLEAR_COLOR;
    STENCILTEST_ON; gl::StencilMask(0b0000);


    /// Render Lighting and Reflections
    BLEND_OFF; graph->render_skybox(false);
    BLEND_OFF; graph->render_ambient(false);
    BLEND_ONEONE; graph->render_skylight(false);
    BLEND_ONEONE; graph->render_spotlights(false);
    BLEND_ONEONE; graph->render_pointlights(false);
    graph->render_reflections();


    /// Write HDR Luma to Alpha
    CLIP_OFF; BLEND_OFF; CULLFACE_OFF;
    DEPTHTEST_OFF; STENCILTEST_OFF;
    TX.hdr->bind(gl::TEXTURE0);
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.gnrc_lumalpha);
    FB.hdr->use(); VIEWPORT_FULL;
    sq::draw_screen_quad();


    /// Bloom
    if (INFO.hdrbEnable == true) {
        VIEWPORT_QTER;
        pipeline->use_shader(*FS.prty_hdr_highs); FB.bloomA->use();
        sq::draw_screen_quad(); TX.bloomA->bind(gl::TEXTURE1);
        pipeline->use_shader(*FS.prty_hdr_blurh); FB.bloomB->use();
        sq::draw_screen_quad(); TX.bloomB->bind(gl::TEXTURE1);
        pipeline->use_shader(*FS.prty_hdr_blurv); FB.bloomA->use();
        sq::draw_screen_quad(); TX.bloomA->bind(gl::TEXTURE1);
        pipeline->use_shader(*FS.prty_hdr_blurh); FB.bloomB->use();
        sq::draw_screen_quad(); TX.bloomB->bind(gl::TEXTURE1);
        pipeline->use_shader(*FS.prty_hdr_blurv); FB.bloomA->use();
        sq::draw_screen_quad(); TX.bloomA->bind(gl::TEXTURE1);
    }


    /// HDR Tonemapping
    TX.hdr->bind(gl::TEXTURE0);
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.prty_hdr_tones);
    FB.basic->use(); VIEWPORT_FULL;
    sq::draw_screen_quad();


    /// Antialiasing and Write to Screen
    TX.basic->bind(gl::TEXTURE0);
    if (INFO.fxaaEnable == true) {
        pipeline->use_shader(*FS.gnrc_lumalpha);
        sq::draw_screen_quad();
        pipeline->use_shader(*FS.prty_fxaa_fxaa);
    } else pipeline->use_shader(*FS.gnrc_passthru);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    sq::draw_screen_quad();


    /// Vignetting Effect
    if (INFO.vgntEnable == true) {
        pipeline->use_shader(*FS.prty_vignette);
        BLEND_ALPHA; sq::draw_screen_quad();
    }

    gl::BindProgramPipeline(0);
    gl::BindVertexArray(0);
}


void SceneMain::update_settings() {
    INFO.fullSize = appBase->get_size();
    INFO.halfSize = INFO.fullSize / 2u;
    INFO.qterSize = INFO.fullSize / 4u;
    INFO.fPixSize = 1.f / vec2(INFO.fullSize);
    INFO.hPixSize = 1.f / vec2(INFO.halfSize);
    INFO.qPixSize = 1.f / vec2(INFO.qterSize);

    graph->INFO.fullSize = INFO.fullSize;
    graph->INFO.halfSize = INFO.halfSize;
    graph->INFO.qterSize = INFO.qterSize;
    graph->INFO.fPixSize = INFO.fPixSize;
    graph->INFO.hPixSize = INFO.hPixSize;
    graph->INFO.qPixSize = INFO.qPixSize;

    TX.depth->resize(INFO.fullSize);
    TX.diff->resize(INFO.fullSize);
    TX.surf->resize(INFO.fullSize);
    TX.norm->resize(INFO.fullSize);
    TX.spec->resize(INFO.fullSize);
    TX.hdr->resize(INFO.fullSize);
    TX.basic->resize(INFO.fullSize);
    TX.ssaoA->resize(INFO.halfSize);
    TX.ssaoB->resize(INFO.halfSize);
    TX.bloomA->resize(INFO.qterSize);
    TX.bloomB->resize(INFO.qterSize);

    camera->size = vec2(INFO.fullSize);

    INFO.ssaoEnable = bool(settings->crnt<int>("ssaoMode"));
    INFO.hdrbEnable = bool(settings->crnt<int>("hdrbMode"));
    INFO.fxaaEnable = bool(settings->crnt<int>("fxaaMode"));
    INFO.vgntEnable = bool(settings->crnt<int>("vignMode"));
    INFO.shadFltr = bool(settings->crnt<int>("shadFltr"));
    INFO.shadMult = uint(glm::pow(2, settings->crnt<int>("shadQlty")));
    INFO.viewDist = float(settings->crnt<float>("viewDist"));

    graph->INFO.shadFltr = INFO.shadFltr;
    graph->INFO.shadMult = INFO.shadMult;
    graph->INFO.viewDist = INFO.viewDist;
    camera->range.y = INFO.viewDist;

    graph->update_settings();
    reload_shaders();
}


void SceneMain::reload_shaders() {
    int shadFltr = settings->crnt<int>("shadFltr");
    int shadQlty = settings->crnt<int>("shadQlty");
    int ssaoMode = settings->crnt<int>("ssaoMode");
    int hdrbMode = settings->crnt<int>("hdrbMode");
    int fxaaMode = settings->crnt<int>("fxaaMode");

    /// Shadows
    string defines = "#define SHADFLTR " + std::to_string(shadFltr) + "\n"
                   + "#define SHADQLTY " + std::to_string(shadQlty);
    string definesShad = defines + "\n#define SHADOW";
    string definesSpec = defines + "\n#define SPECULAR";
    string definesBoth = defines + "\n#define SHADOW\n#define SPECULAR";
    FS.shds_skylight->load(preprocs->load("shades/skylight_fs", defines));
    FS.shds_spot_none->load(preprocs->load("shades/spotlight_fs", defines));
    FS.shds_spot_shad->load(preprocs->load("shades/spotlight_fs", definesShad));
    FS.shds_spot_spec->load(preprocs->load("shades/spotlight_fs", definesSpec));
    FS.shds_spot_both->load(preprocs->load("shades/spotlight_fs", definesBoth));
    FS.shds_point_none->load(preprocs->load("shades/pointlight_fs", defines));
    FS.shds_point_shad->load(preprocs->load("shades/pointlight_fs", definesShad));
    FS.shds_point_spec->load(preprocs->load("shades/pointlight_fs", definesSpec));
    FS.shds_point_both->load(preprocs->load("shades/pointlight_fs", definesBoth));

    defines += "\n#define REFLECT"; definesShad += "\n#define REFLECT";
    FS.shds_skylight_refl->load(preprocs->load("shades/skylight_fs", defines));
    FS.shds_spot_none_refl->load(preprocs->load("shades/spotlight_fs", defines));
    FS.shds_spot_shad_refl->load(preprocs->load("shades/spotlight_fs", definesShad));
    FS.shds_point_none_refl->load(preprocs->load("shades/pointlight_fs", defines));
    FS.shds_point_shad_refl->load(preprocs->load("shades/pointlight_fs", definesShad));

    /// SSAO
    defines = "#define PIXSIZE " + glm::to_string(INFO.hPixSize);
    if (ssaoMode == 0) FS.shds_ambient->load(preprocs->load("shades/ambient_fs"));
    else FS.shds_ambient->load(preprocs->load("shades/ambient_fs", "#define SSAO"));
    if (ssaoMode == 1) FS.prty_ssao_ssao->load(preprocs->load("pretty/ssao/ssao_fs", defines)),
                       FS.prty_ssao_blur->load(preprocs->load("pretty/ssao/blur_fs", defines));
    if (ssaoMode == 2) FS.prty_ssao_ssao->load(preprocs->load("pretty/ssao/ssao_fs", defines+"\n#define HIGH")),
                       FS.prty_ssao_blur->load(preprocs->load("pretty/ssao/blur_fs", defines+"\n#define HIGH"));

    /// HDR
    defines = "#define PIXSIZE " + glm::to_string(INFO.qPixSize);
    if (hdrbMode == 0) FS.prty_hdr_tones->load(preprocs->load("pretty/hdr/tones_fs"));
    else FS.prty_hdr_tones->load(preprocs->load("pretty/hdr/tones_fs", "#define HDRB"));
    if (hdrbMode == 1) FS.prty_hdr_blurh->load(preprocs->load("pretty/hdr/blurh_fs", defines)),
                       FS.prty_hdr_blurv->load(preprocs->load("pretty/hdr/blurv_fs", defines));
    if (hdrbMode == 2) FS.prty_hdr_blurh->load(preprocs->load("pretty/hdr/blurh_fs", defines+"\n#define HIGH")),
                       FS.prty_hdr_blurv->load(preprocs->load("pretty/hdr/blurv_fs", defines+"\n#define HIGH"));

    /// FXAA
    defines = "#define PIXSIZE " + glm::to_string(INFO.fPixSize);
    if (fxaaMode == 1) FS.prty_fxaa_fxaa->load(preprocs->load("pretty/fxaa/fxaa_fs", defines));
    if (fxaaMode == 2) FS.prty_fxaa_fxaa->load(preprocs->load("pretty/fxaa/fxaa_fs", defines+"\n#define HIGH"));
}
