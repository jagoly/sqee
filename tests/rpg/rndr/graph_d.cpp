#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/settings.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/gl/drawing.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/cameras.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/render/skeleton.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include "../main/camera.hpp"
#include "../wcoe/world.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/pointlight.hpp"
#include "../wcoe/obj/spotlight.hpp"
#include "../wcoe/obj/reflector.hpp"
#include "../wcoe/obj/emitter.hpp"
#include "../wcoe/obj/liquid.hpp"
#include "../wcoe/obj/decal.hpp"
#include "graph.hpp"

using namespace sqt;
using namespace rndr;

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


void Graph::render_skybox_base() {
    if (!world->skybox.PROP_enabled) return;
    pipeline->use_shader(*VS.defr_skybox_base);
    pipeline->use_shader(*FS.defr_skybox);
    world->skybox.tex->bind(gl::TEXTURE5);
    world->skybox.ubo->bind(1);

    STENCILTEST_ON; gl::StencilMask(0b0000);
    gl::StencilFunc(gl::EQUAL, 0b0001, 0b0101);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    sq::draw_screen_quad();
}


void Graph::render_skybox_refl() {
    if (!world->skybox.PROP_enabled) return;
    pipeline->use_shader(*VS.defr_skybox_refl);
    pipeline->use_shader(*FS.defr_skybox);
    world->skybox.tex->bind(gl::TEXTURE5);
    world->skybox.ubo->bind(1);

    STENCILTEST_ON; gl::StencilMask(0b0000);
    gl::StencilFunc(gl::EQUAL, 0b0011, 0b0111);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    sq::draw_screen_quad();
}


void Graph::render_ambient_base() {
    if (!world->ambient.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_ambient_base);
    world->ambient.ubo->bind(1);

    STENCILTEST_ON; gl::StencilMask(0b0000);
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    sq::draw_screen_quad();
}


void Graph::render_ambient_refl() {
    if (!world->ambient.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_ambient_refl);
    world->ambient.ubo->bind(1);

    STENCILTEST_ON; gl::StencilMask(0b0000);
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    sq::draw_screen_quad();
}


//void Graph::render_ambient_part() {
//    if (!world->ambient.PROP_enabled) return;
//    pipeline->use_shader(*VS.gnrc_screen);
//    pipeline->use_shader(*FS.defr_ambient_part);
//    world->ambient.ubo->bind(1);

//    STENCILTEST_ON; gl::StencilMask(0b0000);
//    gl::StencilFunc(gl::EQUAL, 0b0001, 0b0001);
//    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
//    sq::draw_screen_quad();
//}


void Graph::render_skylight_base() {
    if (!world->skylight.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_skylight_base);
    world->skylight.texA->bind(gl::TEXTURE8);
    world->skylight.ubo->bind(1);

    STENCILTEST_ON; gl::StencilMask(0b0000);
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    sq::draw_screen_quad();
}


void Graph::render_skylight_refl() {
    if (!world->skylight.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_skylight_refl);
    world->skylight.texB->bind(gl::TEXTURE8);
    world->skylight.ubo->bind(1);

    STENCILTEST_ON; gl::StencilMask(0b0000);
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    sq::draw_screen_quad();
}


//void Graph::render_skylight_part() {
//    if (!world->skylight.PROP_enabled) return;

//    STENCILTEST_ON; gl::StencilMask(0b0000);
//    gl::StencilFunc(gl::EQUAL, 0b0001, 0b0001);
//    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
//    world->skylight.texA->bind(gl::TEXTURE8);
//    world->skylight.ubo->bind(1);

//    BLEND_OFF; DEPTHTEST_OFF;
//    FB.pshadA->use(); CLEAR_COLOR;
//    FB.pshadB->use(); CLEAR_COLOR;

//    TX.pshadA->bind(gl::TEXTURE8);
//    pipeline->use_shader(*VS.gnrc_screen);
//    pipeline->use_shader(*FS.defr_skylight_pshad);
//    FB.pshadA->use(); sq::draw_screen_quad();

//    const sq::Shader &blurH = *FS.defr_partshadblurh, &blurV = *FS.defr_partshadblurv;
//    pipeline->use_shader(blurH); FB.pshadB->use(); sq::draw_screen_quad(); TX.pshadB->bind();
//    pipeline->use_shader(blurV); FB.pshadA->use(); sq::draw_screen_quad(); TX.pshadA->bind();
//    pipeline->use_shader(blurH); FB.pshadB->use(); sq::draw_screen_quad(); TX.pshadB->bind();
//    pipeline->use_shader(blurV); FB.pshadA->use(); sq::draw_screen_quad(); TX.pshadA->bind();

//    pipeline->use_shader(*FS.defr_skylight_part);
//    FB.hdrPart->use(); BLEND_ONEONE;
//    sq::draw_screen_quad();
//}


void Graph::render_spotlights_base() {
    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.lock();
        if (sq::frus_in_frus(light.frus, camera->frus)) continue;

        light.ubo->bind(1); sq::Shader *shadeFS;
        if (!light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_spot_none_base;
        if ( light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_spot_shad_base;
        if (!light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_spot_spec_base;
        if ( light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_spot_both_base;
        if (light.PROP_shadow) light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_stencil_base);
        VS.gbuf_stencil_base->set_mat("matrix", light.modelMat);
        gl::StencilMask(0b1000); CLEAR_STENC;
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b0101);
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        DEPTHTEST_RO; sq::draw_volume_cone();
        DEPTHTEST_OFF; gl::StencilMask(0);

        pipeline->use_shader(*shadeFS);
        pipeline->use_shader(*VS.gnrc_screen);
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b1101);
        gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
        sq::draw_screen_quad();
    }
}


void Graph::render_spotlights_refl() {
    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.lock();
        if (sq::frus_in_frus(light.frus, crntRflct->frus)) continue;

        light.ubo->bind(1); sq::Shader *shadeFS;
        if (light.PROP_shadow == false) shadeFS = FS.defr_spot_none_refl;
        else shadeFS = FS.defr_spot_shad_refl, light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_stencil_refl);
        VS.gbuf_stencil_refl->set_mat("matrix", light.modelMat);
        gl::StencilMask(0b1000); CLEAR_STENC;
        gl::StencilFunc(gl::EQUAL, 0b1111, 0b0111);
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        DEPTHTEST_RO; sq::draw_volume_cone();
        DEPTHTEST_OFF; gl::StencilMask(0);

        pipeline->use_shader(*shadeFS);
        pipeline->use_shader(*VS.gnrc_screen);
        gl::StencilFunc(gl::EQUAL, 0b1111, 0b1111);
        gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
        sq::draw_screen_quad();
    }
}


void Graph::render_pointlights_base() {
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.lock();
        if (sq::sphr_in_frus(light.sphere, camera->frus)) continue;

        light.ubo->bind(1); sq::Shader *shadeFS;
        if (!light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_point_none_base;
        if ( light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_point_shad_base;
        if (!light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_point_spec_base;
        if ( light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_point_both_base;
        if (light.PROP_shadow) light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_stencil_base);
        VS.gbuf_stencil_base->set_mat("matrix", light.modelMat);
        gl::StencilMask(0b1000); CLEAR_STENC;
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b0101);
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        DEPTHTEST_RO; sq::draw_volume_sphr();
        DEPTHTEST_OFF; gl::StencilMask(0);

        pipeline->use_shader(*shadeFS);
        pipeline->use_shader(*VS.gnrc_screen);
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b1101);
        gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
        sq::draw_screen_quad();
    }
}


void Graph::render_pointlights_refl() {
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.lock();
        if (sq::sphr_in_frus(light.sphere, crntRflct->frus)) continue;

        light.ubo->bind(1); sq::Shader *shadeFS;
        if (light.PROP_shadow == false) shadeFS = FS.defr_point_none_refl;
        else shadeFS = FS.defr_point_shad_refl, light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_stencil_refl);
        VS.gbuf_stencil_refl->set_mat("matrix", light.modelMat);
        gl::StencilMask(0b1000); CLEAR_STENC;
        gl::StencilFunc(gl::EQUAL, 0b1111, 0b0111);
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        DEPTHTEST_RO; sq::draw_volume_sphr();
        DEPTHTEST_OFF; gl::StencilMask(0);

        pipeline->use_shader(*shadeFS);
        pipeline->use_shader(*VS.gnrc_screen);
        gl::StencilFunc(gl::EQUAL, 0b1111, 0b1111);
        gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
        sq::draw_screen_quad();
    }
}
