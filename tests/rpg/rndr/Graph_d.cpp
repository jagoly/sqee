#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_4_1.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/objects/ModelStatic.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/PointLight.hpp"
#include "../wcoe/objects/SpotLight.hpp"
#include "../wcoe/objects/Reflector.hpp"
#include "../wcoe/objects/Emitter.hpp"
#include "../wcoe/objects/Liquid.hpp"
#include "../wcoe/objects/Decal.hpp"
#include "Graph.hpp"

using namespace sqt::rndr;

void Graph::render_skybox_base() {
    if (!world->skybox.PROP_enabled) return;
    pipeline->use_shader(*VS.defr_base_skybox);
    pipeline->use_shader(*FS.defr_skybox);
    world->skybox.tex->bind(gl::TEXTURE0);
    world->skybox.ubo->bind(1);

    sq::BLEND_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0001, 0b0101);
    sq::draw_screen_quad();
}


void Graph::render_skybox_refl() {
    if (!world->skybox.PROP_enabled) return;
    pipeline->use_shader(*VS.defr_refl_skybox);
    pipeline->use_shader(*FS.defr_skybox);
    world->skybox.tex->bind(gl::TEXTURE0);
    world->skybox.ubo->bind(1);

    sq::BLEND_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0011, 0b0111);
    sq::draw_screen_quad();
}


void Graph::render_ambient_base() {
    if (!world->ambient.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_base_ambient);
    TX.ssaoB->bind(gl::TEXTURE0);
    TX.depHalf->bind(gl::TEXTURE1);
    world->ambient.ubo->bind(1);

    sq::BLEND_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    sq::draw_screen_quad();
}


void Graph::render_ambient_refl() {
    if (!world->ambient.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_refl_ambient);
    world->ambient.ubo->bind(1);

    sq::BLEND_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
    sq::draw_screen_quad();
}


void Graph::render_skylight_base() {
    if (!world->skylight.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_base_skylight);
    world->skylight.texDepthA->bind(gl::TEXTURE8);
    world->skylight.ubo->bind(1);

    sq::BLEND_ON(); sq::BLEND_ONEONE();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    sq::STENCIL_KEEP(); sq::draw_screen_quad();
}


void Graph::render_skylight_refl() {
    if (!world->skylight.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_refl_skylight);
    world->skylight.texDepthB->bind(gl::TEXTURE8);
    world->skylight.ubo->bind(1);

    sq::BLEND_ON(); sq::BLEND_ONEONE();
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
    sq::STENCIL_KEEP(); sq::draw_screen_quad();
}


void Graph::render_spotlights_base() {
    sq::BLEND_ON(); sq::BLEND_ONEONE();
    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.lock();
        if (sq::frus_in_frus(light.frus, camera->frus)) continue;

        light.ubo->bind(1); sq::Shader *shadeFS;
        if (!light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_base_spot_none;
        if ( light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_base_spot_shad;
        if (!light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_base_spot_spec;
        if ( light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_base_spot_both;
        if (light.PROP_shadow) light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_base_stencil);
        VS.gbuf_base_stencil->set_mat("matrix", light.modelMat);
        gl::StencilMask(0b1000); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b0101);
        sq::DEPTH_ON(); sq::DEPTH_READ();
        sq::draw_volume_cone(); sq::DEPTH_OFF();

        pipeline->use_shader(*shadeFS);
        pipeline->use_shader(*VS.gnrc_screen);
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b1101);
        sq::STENCIL_KEEP(); sq::draw_screen_quad();
    }
}


void Graph::render_spotlights_refl() {
    sq::BLEND_ON(); sq::BLEND_ONEONE();
    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.lock();
        if (sq::frus_in_frus(light.frus, crntRflct->frus)) continue;

        light.ubo->bind(1); sq::Shader *shadeFS;
        if (light.PROP_shadow == false) shadeFS = FS.defr_refl_spot_none;
        else shadeFS = FS.defr_refl_spot_shad, light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_refl_stencil);
        VS.gbuf_refl_stencil->set_mat("matrix", light.modelMat);
        gl::StencilMask(0b1000); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b1111, 0b0111);
        sq::DEPTH_ON(); sq::DEPTH_READ();
        sq::draw_volume_cone(); sq::DEPTH_OFF();

        pipeline->use_shader(*shadeFS);
        pipeline->use_shader(*VS.gnrc_screen);
        gl::StencilFunc(gl::EQUAL, 0b1111, 0b1111);
        sq::STENCIL_KEEP(); sq::draw_screen_quad();
    }
}


void Graph::render_pointlights_base() {
    sq::BLEND_ON(); sq::BLEND_ONEONE();
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.lock();
        if (sq::sphr_in_frus(light.sphere, camera->frus)) continue;

        light.ubo->bind(1); sq::Shader *shadeFS;
        if (!light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_base_point_none;
        if ( light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_base_point_shad;
        if (!light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_base_point_spec;
        if ( light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_base_point_both;
        if (light.PROP_shadow) light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_base_stencil);
        VS.gbuf_base_stencil->set_mat("matrix", light.modelMat);
        gl::StencilMask(0b1000); sq::CLEAR_STENC();
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b0101);
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        sq::DEPTH_ON(); sq::DEPTH_READ();
        sq::draw_volume_sphr(); sq::DEPTH_OFF();

        pipeline->use_shader(*shadeFS);
        pipeline->use_shader(*VS.gnrc_screen);
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b1101);
        sq::STENCIL_KEEP(); sq::draw_screen_quad();
    }
}


void Graph::render_pointlights_refl() {
    sq::BLEND_ON(); sq::BLEND_ONEONE();
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.lock();
        if (sq::sphr_in_frus(light.sphere, crntRflct->frus)) continue;

        light.ubo->bind(1); sq::Shader *shadeFS;
        if (light.PROP_shadow == false) shadeFS = FS.defr_refl_point_none;
        else shadeFS = FS.defr_refl_point_shad, light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_refl_stencil);
        VS.gbuf_refl_stencil->set_mat("matrix", light.modelMat);
        gl::StencilMask(0b1000); sq::CLEAR_STENC();
        gl::StencilFunc(gl::EQUAL, 0b1111, 0b0111);
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        sq::DEPTH_ON(); sq::DEPTH_READ();
        sq::draw_volume_sphr(); sq::DEPTH_OFF();

        pipeline->use_shader(*shadeFS);
        pipeline->use_shader(*VS.gnrc_screen);
        gl::StencilFunc(gl::EQUAL, 0b1111, 0b1111);
        sq::STENCIL_KEEP(); sq::draw_screen_quad();
    }
}
