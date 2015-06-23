#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/SettingsMap.hpp>
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
#include "../wcoe/obj/ModelStatic.hpp"
#include "../wcoe/obj/ModelSkelly.hpp"
#include "../wcoe/obj/PointLight.hpp"
#include "../wcoe/obj/SpotLight.hpp"
#include "../wcoe/obj/Reflector.hpp"
#include "../wcoe/obj/Emitter.hpp"
#include "../wcoe/obj/Liquid.hpp"
#include "../wcoe/obj/Decal.hpp"
#include "Graph.hpp"

using namespace sqt::rndr;

void Graph::render_skybox_base() {
    if (!world->skybox.PROP_enabled) return;
    pipeline->use_shader(*VS.defr_skybox_base);
    pipeline->use_shader(*FS.defr_skybox);
    world->skybox.tex->bind(gl::TEXTURE5);
    world->skybox.ubo->bind(1);

    sq::BLEND_OFF();
    gl::StencilFunc(gl::EQUAL, 0b0001, 0b0101);
    sq::STENCIL_KEEP(); sq::draw_screen_quad();
}


void Graph::render_skybox_refl() {
    if (!world->skybox.PROP_enabled) return;
    pipeline->use_shader(*VS.defr_skybox_refl);
    pipeline->use_shader(*FS.defr_skybox);
    world->skybox.tex->bind(gl::TEXTURE5);
    world->skybox.ubo->bind(1);

    sq::BLEND_OFF();
    gl::StencilFunc(gl::EQUAL, 0b0011, 0b0111);
    sq::STENCIL_KEEP(); sq::draw_screen_quad();
}


void Graph::render_ambient_base() {
    if (!world->ambient.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_ambient_base);
    world->ambient.ubo->bind(1);

    sq::BLEND_OFF();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    sq::STENCIL_KEEP(); sq::draw_screen_quad();
}


void Graph::render_ambient_refl() {
    if (!world->ambient.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_ambient_refl);
    world->ambient.ubo->bind(1);

    sq::BLEND_OFF();
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
    sq::STENCIL_KEEP(); sq::draw_screen_quad();
}


void Graph::render_skylight_base() {
    if (!world->skylight.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_skylight_base);
    world->skylight.texA->bind(gl::TEXTURE8);
    world->skylight.ubo->bind(1);

    sq::BLEND_ON(); sq::BLEND_ONEONE();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    sq::STENCIL_KEEP(); sq::draw_screen_quad();
}


void Graph::render_skylight_refl() {
    if (!world->skylight.PROP_enabled) return;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.defr_skylight_refl);
    world->skylight.texB->bind(gl::TEXTURE8);
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
        if (!light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_spot_none_base;
        if ( light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_spot_shad_base;
        if (!light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_spot_spec_base;
        if ( light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_spot_both_base;
        if (light.PROP_shadow) light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_stencil_base);
        VS.gbuf_stencil_base->set_mat("matrix", light.modelMat);
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
        if (light.PROP_shadow == false) shadeFS = FS.defr_spot_none_refl;
        else shadeFS = FS.defr_spot_shad_refl, light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_stencil_refl);
        VS.gbuf_stencil_refl->set_mat("matrix", light.modelMat);
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
        if (!light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_point_none_base;
        if ( light.PROP_shadow && !light.PROP_specular) shadeFS = FS.defr_point_shad_base;
        if (!light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_point_spec_base;
        if ( light.PROP_shadow &&  light.PROP_specular) shadeFS = FS.defr_point_both_base;
        if (light.PROP_shadow) light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_stencil_base);
        VS.gbuf_stencil_base->set_mat("matrix", light.modelMat);
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
        if (light.PROP_shadow == false) shadeFS = FS.defr_point_none_refl;
        else shadeFS = FS.defr_point_shad_refl, light.tex->bind(gl::TEXTURE8);

        pipeline->disable_stages(0, 0, 1);
        pipeline->use_shader(*VS.gbuf_stencil_refl);
        VS.gbuf_stencil_refl->set_mat("matrix", light.modelMat);
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
