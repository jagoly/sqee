#include <sqee/app/Settings.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/SkyBox.hpp"
#include "../wcoe/Ambient.hpp"
#include "../wcoe/SkyLight.hpp"

#include "Gbuffers.hpp"
#include "Pretties.hpp"
#include "Lighting.hpp"

#include "../components/Transform.hpp"
#include "../components/SpotLight.hpp"
#include "../components/PointLight.hpp"

#include <sqee/misc/StringCast.hpp>

using namespace sqt::rndr;

Lighting::Lighting(const Renderer& _renderer) : renderer(_renderer) {

    FB_baseHdr.draw_buffers({gl::COLOR_ATTACHMENT0});

    renderer.preprocs(VS_defr_base_skybox, "deferred/base/skybox_vs");
    renderer.preprocs(VS_defr_refl_skybox, "deferred/refl/skybox_vs");
    renderer.preprocs(FS_defr_refl_ambient, "deferred/refl/ambient_fs");
    renderer.preprocs(FS_defr_skybox, "deferred/skybox_fs");
}


void Lighting::render_lighting_base() {
    FB_baseHdr.bind(); sq::CLEAR_COLOR();
    sq::STENCIL_ON(); sq::CULLFACE_OFF();
    sq::DEPTH_READ(); sq::BLEND_ONEONE();
    sq::VIEWPORT(INFO_fullSize);
    gl::StencilMask(0b0010);

    if (renderer.world.skybox->PROP_enabled == true) {
        renderer.world.skybox->tex.bind(gl::TEXTURE0);
        renderer.pipeline.use_shader(VS_defr_base_skybox);
        renderer.pipeline.use_shader(FS_defr_skybox);
        renderer.world.skybox->ubo.bind(1u);

        sq::DEPTH_OFF(); sq::STENCIL_KEEP();
        gl::StencilFunc(gl::EQUAL, 0b0001, 0b0000);
        sq::BLEND_OFF(); sq::draw_screen_quad();
    }

    if (renderer.world.ambient->PROP_enabled == true) {
        if (INFO_ssao > 0)
            renderer.pretties->TEX_ssao.bind(gl::TEXTURE0),
            renderer.gbuffers->TEX_depHalf.bind(gl::TEXTURE1);
        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_base_ambient);
        renderer.world.ambient->ubo.bind(1u);

        sq::DEPTH_OFF(); sq::STENCIL_KEEP();
        gl::StencilFunc(gl::EQUAL, 0b0001, 0b0001);
        sq::BLEND_OFF(); sq::draw_screen_quad();
    }

    if (renderer.world.skylight->PROP_enabled == true) {
        renderer.world.skylight->texA.bind(gl::TEXTURE8);
        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_base_skylight);
        renderer.world.skylight->ubo.bind(1u);

        sq::DEPTH_OFF(); sq::STENCIL_KEEP();
        gl::StencilFunc(gl::EQUAL, 0b0001, 0b0001);
        sq::BLEND_ON(); sq::draw_screen_quad();
    }

    for (const auto light : renderer.cameraData.spotLightShadowVec) {
        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_base);
        renderer.VS_stencil_base.set_mat("matrix", light->modelMat);
        sq::DEPTH_ON(); sq::BLEND_ON(); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0001);
        sq::draw_volume_cone(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_base_spot_both);
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0011);
        light->ubo.bind(1u); sq::STENCIL_KEEP();
        light->tex.bind(gl::TEXTURE8);
        sq::draw_screen_quad();
    }

    for (const auto light : renderer.cameraData.spotLightNoShadowVec) {
        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_base);
        renderer.VS_stencil_base.set_mat("matrix", light->modelMat);
        sq::DEPTH_ON(); sq::BLEND_ON(); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0001);
        sq::draw_volume_cone(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_base_spot_spec);
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0011);
        light->ubo.bind(1u); sq::STENCIL_KEEP();
        sq::draw_screen_quad();
    }

    for (const auto light : renderer.cameraData.pointLightShadowVec) {
        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_base);
        renderer.VS_stencil_base.set_mat("matrix", light->modelMat);
        sq::DEPTH_ON(); sq::BLEND_ON(); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0001);
        sq::draw_volume_sphr(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_base_point_both);
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0011);
        light->ubo.bind(1u); sq::STENCIL_KEEP();
        light->tex.bind(gl::TEXTURE8);
        sq::draw_screen_quad();
    }

    for (const auto light : renderer.cameraData.pointLightNoShadowVec) {
        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_base);
        renderer.VS_stencil_base.set_mat("matrix", light->modelMat);
        sq::DEPTH_ON(); sq::BLEND_ON(); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0001);
        sq::draw_volume_sphr(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_base_point_spec);
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0011);
        light->ubo.bind(1u); sq::STENCIL_KEEP();
        sq::draw_screen_quad();

        std::cout << sq::chai_string(light->modelMat) << std::endl;
    }
}


void Lighting::render_lighting_refl(const ReflectData& _data) {
    if (renderer.world.skybox->PROP_enabled == true) {
        renderer.world.skybox->tex.bind(gl::TEXTURE0);
        renderer.pipeline.use_shader(VS_defr_refl_skybox);
        renderer.pipeline.use_shader(FS_defr_skybox);
        renderer.world.skybox->ubo.bind(1u);

        sq::DEPTH_OFF(); sq::STENCIL_KEEP();
        gl::StencilFunc(gl::EQUAL, 0b0100, 0b0101);
        sq::BLEND_OFF(); sq::draw_screen_quad();
    }

    if (renderer.world.ambient->PROP_enabled == true) {
        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_refl_ambient);
        renderer.world.ambient->ubo.bind(1u);

        sq::DEPTH_OFF(); sq::STENCIL_KEEP();
        gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
        sq::BLEND_OFF(); sq::draw_screen_quad();
    }

    if (renderer.world.skylight->PROP_enabled == true) {
        renderer.world.skylight->texB.bind(gl::TEXTURE8);
        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_refl_skylight);
        renderer.world.skylight->ubo.bind(1u);

        sq::DEPTH_OFF(); sq::STENCIL_KEEP();
        gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
        sq::BLEND_ON(); sq::draw_screen_quad();
    }

    for (const auto lightC : _data.spotLightShadowVec) {
        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_refl);
        renderer.VS_stencil_refl.set_mat("matrix", lightC->modelMat);
        sq::DEPTH_ON(); sq::BLEND_ON(); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b0111, 0b0101);
        sq::draw_volume_cone(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_refl_spot_shad);
        gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
        sq::STENCIL_KEEP(); lightC->ubo.bind(1u);
        lightC->tex.bind(gl::TEXTURE8);
        sq::draw_screen_quad();
    }

    for (const auto lightC : _data.spotLightNoShadowVec) {
        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_refl);
        renderer.VS_stencil_refl.set_mat("matrix", lightC->modelMat);
        sq::DEPTH_ON(); sq::BLEND_ON(); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b0111, 0b0101);
        sq::draw_volume_cone(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_refl_spot_none);
        gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
        sq::STENCIL_KEEP(); lightC->ubo.bind(1u);
        sq::draw_screen_quad();
    }

    for (const auto lightC : _data.pointLightShadowVec) {
        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_refl);
        renderer.VS_stencil_refl.set_mat("matrix", lightC->modelMat);
        sq::DEPTH_ON(); sq::BLEND_ON(); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b0111, 0b0101);
        sq::draw_volume_sphr(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_refl_point_shad);
        gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
        sq::STENCIL_KEEP(); lightC->ubo.bind(1u);
        lightC->tex.bind(gl::TEXTURE8);
        sq::draw_screen_quad();
    }

    for (const auto lightC : _data.pointLightNoShadowVec) {
        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_refl);
        renderer.VS_stencil_refl.set_mat("matrix", lightC->modelMat);
        sq::DEPTH_ON(); sq::BLEND_ON(); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b0111, 0b0101);
        sq::draw_volume_sphr(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_defr_refl_point_none);
        gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
        sq::STENCIL_KEEP(); lightC->ubo.bind(1u);
        sq::draw_screen_quad();
    }
}


void Lighting::update_settings() {
    INFO_fullSize = Vec2U(renderer.settings.get<int>("app_width"),
                          renderer.settings.get<int>("app_height"));
    INFO_halfSize = INFO_fullSize / 2u;

    INFO_ssao = renderer.settings.get<int>("rpg_ssao");

    bool shadLarge = renderer.settings.get<bool>("rpg_shadlarge");
    bool shadFilter = renderer.settings.get<bool>("rpg_shadfilter");

    string defLightBase = "";
    if (shadLarge) defLightBase += "#define LARGE\n";
    if (shadFilter) defLightBase += "#define FILTER";
    string defLightShad = defLightBase + "\n#define SHADOW";
    string defLightSpec = defLightBase + "\n#define SPECULAR";
    string defLightBoth = defLightShad + "\n#define SPECULAR";

    renderer.preprocs(FS_defr_base_skylight, "deferred/base/skylight_fs", defLightBase);
    renderer.preprocs(FS_defr_base_spot_none, "deferred/base/spotlight_fs", defLightBase);
    renderer.preprocs(FS_defr_base_spot_shad, "deferred/base/spotlight_fs", defLightShad);
    renderer.preprocs(FS_defr_base_spot_spec, "deferred/base/spotlight_fs", defLightSpec);
    renderer.preprocs(FS_defr_base_spot_both, "deferred/base/spotlight_fs", defLightBoth);
    renderer.preprocs(FS_defr_base_point_none, "deferred/base/pointlight_fs", defLightBase);
    renderer.preprocs(FS_defr_base_point_shad, "deferred/base/pointlight_fs", defLightShad);
    renderer.preprocs(FS_defr_base_point_spec, "deferred/base/pointlight_fs", defLightSpec);
    renderer.preprocs(FS_defr_base_point_both, "deferred/base/pointlight_fs", defLightBoth);
    renderer.preprocs(FS_defr_refl_skylight, "deferred/refl/skylight_fs", defLightBase);
    renderer.preprocs(FS_defr_refl_spot_none, "deferred/refl/spotlight_fs", defLightBase);
    renderer.preprocs(FS_defr_refl_spot_shad, "deferred/refl/spotlight_fs", defLightShad);
    renderer.preprocs(FS_defr_refl_point_none, "deferred/refl/pointlight_fs", defLightBase);
    renderer.preprocs(FS_defr_refl_point_shad, "deferred/refl/pointlight_fs", defLightShad);

    if (INFO_ssao == false) renderer.preprocs(FS_defr_base_ambient, "deferred/base/ambient_fs");
    else renderer.preprocs(FS_defr_base_ambient, "deferred/base/ambient_fs", "#define SSAO");

    TEX_baseHdr.allocate_storage(INFO_fullSize, false);

    FB_baseHdr.attach(gl::COLOR_ATTACHMENT0, TEX_baseHdr);
    FB_baseHdr.attach(gl::DEPTH_STENCIL_ATTACHMENT, renderer.gbuffers->TEX_baseDpSt);
}
