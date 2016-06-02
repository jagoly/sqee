#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/SkyLight.hpp"

#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/SpotLight.hpp"
#include "../components/PointLight.hpp"

#include "Shadows.hpp"

using namespace sqt;

Renderer::Shadows::Shadows(const Renderer& _renderer)
    : renderer(_renderer), options(renderer.options) {

    VS_shad_simple.add_uniform("matrix"); // mat4
    VS_shad_skelly.add_uniform("matrix"); // mat4

    renderer.preprocs(VS_shad_simple, "shadows/simple_vs");
    renderer.preprocs(VS_shad_skelly, "shadows/skelly_vs");
    renderer.preprocs(FS_shad_punch, "shadows/punch_fs");
}


void Renderer::Shadows::setup_render_state() {
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::BLEND_OFF(); sq::STENCIL_OFF();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
    FB_shadows.bind();
}


void Renderer::Shadows::render_shadows_sky() {
    if (renderer.world.check_SkyLight() == false) return;

    const SkyLightData& data = renderer.skyLightData;
    const SkyLight& light = renderer.world.get_SkyLight();

    sq::VIEWPORT(light.texA.get_size());
    sq::DCLAMP_ON();

    for (uint csm = 0u; csm < 4u; ++csm) {
        FB_shadows.attach(gl::DEPTH_ATTACHMENT, light.texA, csm);
        sq::CLEAR_DEPTH();

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_simple);
        for (const auto* model : data.modelSimpleVecArrA[csm])
            draw_ModelSimple(light.matArrA[csm], model);
        for (const auto* model : data.modelSimpleVecArrA[csm])
            draw_ModelSimple_punch(light.matArrA[csm], model);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_skelly);
        for (const auto* model : data.modelSkellyVecArrA[csm])
            draw_ModelSkelly(light.matArrA[csm], model);
        for (const auto* model : data.modelSkellyPunchVecArrA[csm])
            draw_ModelSkelly_punch(light.matArrA[csm], model);
    }

    for (uint csm = 0u; csm < 2u; ++csm) {
        FB_shadows.attach(gl::DEPTH_ATTACHMENT, light.texB, csm);
        sq::CLEAR_DEPTH();

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_simple);
        for (const auto* model : data.modelSimpleVecArrB[csm])
            draw_ModelSimple(light.matArrB[csm], model);
        for (const auto* model : data.modelSimpleVecArrB[csm])
            draw_ModelSimple_punch(light.matArrB[csm], model);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_skelly);
        for (const auto* model : data.modelSkellyVecArrB[csm])
            draw_ModelSkelly(light.matArrB[csm], model);
        for (const auto* model : data.modelSkellyPunchVecArrB[csm])
            draw_ModelSkelly_punch(light.matArrB[csm], model);
    }

    sq::FRONTFACE(false);
    sq::DCLAMP_OFF();
}


void Renderer::Shadows::render_shadows_spot() {
    for (const SpotLightData& data : renderer.spotLightDataVec) {
        sq::VIEWPORT(data.light->tex.get_size());

        FB_shadows.attach(gl::DEPTH_ATTACHMENT, data.light->tex);
        sq::CLEAR_DEPTH();

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_simple);
        for (const auto* model : data.modelSimpleVec)
            draw_ModelSimple(data.light->matrix, model);
        for (const auto* model : data.modelSimplePunchVec)
            draw_ModelSimple_punch(data.light->matrix, model);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_skelly);
        for (const auto* model : data.modelSkellyVec)
            draw_ModelSkelly(data.light->matrix, model);
        for (const auto* model : data.modelSkellyPunchVec)
            draw_ModelSkelly_punch(data.light->matrix, model);
    }

    sq::FRONTFACE(false);
}


void Renderer::Shadows::render_shadows_point() {
    for (const PointLightData& data : renderer.pointLightDataVec) {
        sq::VIEWPORT(data.light->tex.get_size());

        for (uint face = 0u; face < 6u; ++face) {
            if (data.visibleFaceArr[face] == false) continue;

            FB_shadows.attach(gl::DEPTH_ATTACHMENT, data.light->tex, face);
            sq::CLEAR_DEPTH();

            renderer.pipeline.disable_stages(0, 0, 1);
            renderer.pipeline.use_shader(VS_shad_simple);
            for (const auto* model : data.modelSimpleVecArr[face])
                draw_ModelSimple(data.light->matArr[face], model);
            for (const auto* model : data.modelSimplePunchVecArr[face])
                draw_ModelSimple_punch(data.light->matArr[face], model);

            renderer.pipeline.disable_stages(0, 0, 1);
            renderer.pipeline.use_shader(VS_shad_skelly);
            for (const auto* model : data.modelSkellyVecArr[face])
                draw_ModelSkelly(data.light->matArr[face], model);
            for (const auto* model : data.modelSkellyPunchVecArr[face])
                draw_ModelSkelly_punch(data.light->matArr[face], model);
        }
    }

    sq::FRONTFACE(false);
}


void Renderer::Shadows::update_options() {

}


void Renderer::Shadows::draw_ModelSimple(const Mat4F& _lightMat, const ModelComponent* _model) {
    VS_shad_simple.set_mat<Mat4F>("matrix", _lightMat * _model->matrix);
    sq::FRONTFACE(_model->negScale); _model->mesh->bind_vao();
    _model->mesh->draw_complete();
}

void Renderer::Shadows::draw_ModelSimple_punch(const Mat4F& _lightMat, const ModelComponent* _model) {
    VS_shad_simple.set_mat<Mat4F>("matrix", _lightMat * _model->matrix);
    sq::FRONTFACE(_model->negScale); _model->mesh->bind_vao();
    for (uint i = 0u; i < _model->mesh->mtrlCount; ++i) {
        if (_model->skin->mtrlVec[i].punch == true)
            _model->skin->bind_textures(i, 1, 0, 0),
            renderer.pipeline.use_shader(FS_shad_punch);
        else renderer.pipeline.disable_stages(0, 0, 1);
        _model->mesh->draw_material(i);
    }
}

void Renderer::Shadows::draw_ModelSkelly(const Mat4F& _lightMat, const ModelComponent* _model) {
    VS_shad_skelly.set_mat<Mat4F>("matrix", _lightMat * _model->matrix);
    sq::FRONTFACE(_model->negScale); _model->mesh->bind_vao();
    _model->ubo.bind(1u); _model->mesh->draw_complete();
}

void Renderer::Shadows::draw_ModelSkelly_punch(const Mat4F& _lightMat, const ModelComponent* _model) {
    VS_shad_skelly.set_mat<Mat4F>("matrix", _lightMat * _model->matrix);
    sq::FRONTFACE(_model->negScale); _model->mesh->bind_vao(); _model->ubo.bind(1u);
    for (uint i = 0u; i < _model->mesh->mtrlCount; ++i) {
        if (_model->skin->mtrlVec[i].punch == true)
            _model->skin->bind_textures(i, 1, 0, 0),
            renderer.pipeline.use_shader(FS_shad_punch);
        else renderer.pipeline.disable_stages(0, 0, 1);
        _model->mesh->draw_material(i);
    }
}
