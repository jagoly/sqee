#include <sqee/app/PreProcessor.hpp>
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

using namespace sqt::rndr;

Shadows::Shadows(const Renderer& _renderer) : renderer(_renderer) {

    VS_shad_simple.add_uniform("matrix"); // mat4
    VS_shad_skelly.add_uniform("matrix"); // mat4

    renderer.preprocs(VS_shad_simple, "shadows/simple_vs");
    renderer.preprocs(VS_shad_skelly, "shadows/skelly_vs");
    renderer.preprocs(FS_shad_punch, "shadows/punch_fs");
}


void Shadows::setup_render_state() {
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::BLEND_OFF(); sq::STENCIL_OFF();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
    FB_shadows.bind();
}


void Shadows::render_shadows_sky() {
    const wcoe::SkyLight& light = *renderer.world.skylight;
    const SkyLightData& data = renderer.skyLightData;

    sq::VIEWPORT(light.texA.get_size());
    sq::DCLAMP_ON();

    for (uint csm = 0u; csm < 4u; ++csm) {
        FB_shadows.attach(gl::DEPTH_ATTACHMENT, light.texA, csm);
        sq::CLEAR_DEPTH(); const Mat4F& matrix = light.matArrA[csm];

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_simple);
        for (const auto modelC : data.modelSimpleVecArrA[csm])
            draw_ModelSimple(matrix, modelC);
        for (const auto modelC : data.modelSimpleVecArrA[csm])
            draw_ModelSimple_punch(matrix, modelC);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_skelly);
        for (const auto modelC : data.modelSkellyVecArrA[csm])
            draw_ModelSkelly(matrix, modelC);
        for (const auto modelC : data.modelSkellyPunchVecArrA[csm])
            draw_ModelSkelly_punch(matrix, modelC);
    }

    for (uint csm = 0u; csm < 2u; ++csm) {
        FB_shadows.attach(gl::DEPTH_ATTACHMENT, light.texB, csm);
        sq::CLEAR_DEPTH(); const Mat4F& matrix = light.matArrB[csm];

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_simple);
        for (const auto modelC : data.modelSimpleVecArrB[csm])
            draw_ModelSimple(matrix, modelC);
        for (const auto modelC : data.modelSimpleVecArrB[csm])
            draw_ModelSimple_punch(matrix, modelC);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_skelly);
        for (const auto modelC : data.modelSkellyVecArrB[csm])
            draw_ModelSkelly(matrix, modelC);
        for (const auto modelC : data.modelSkellyPunchVecArrB[csm])
            draw_ModelSkelly_punch(matrix, modelC);
    }

    sq::FRONTFACE(false);
    sq::DCLAMP_OFF();
}


void Shadows::render_shadows_spot() {
    for (const SpotLightData& data : renderer.spotLightDataVec) {
        FB_shadows.attach(gl::DEPTH_ATTACHMENT, data.light->tex);
        sq::CLEAR_DEPTH(); const Mat4F& matrix = data.light->matrix;
        sq::VIEWPORT(data.light->tex.get_size());

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_simple);
        for (const auto modelC : data.modelSimpleVec)
            draw_ModelSimple(matrix, modelC);
        for (const auto modelC : data.modelSimplePunchVec)
            draw_ModelSimple_punch(matrix, modelC);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_skelly);
        for (const auto modelC : data.modelSkellyVec)
            draw_ModelSkelly(matrix, modelC);
        for (const auto modelC : data.modelSkellyPunchVec)
            draw_ModelSkelly_punch(matrix, modelC);
    }

    sq::FRONTFACE(false);
}


void Shadows::render_shadows_point() {
    for (const PointLightData& data : renderer.pointLightDataVec) {
        sq::VIEWPORT(data.light->tex.get_size());

        for (uint face = 0u; face < 6u; ++face) {
            if (data.visibleFaceArr[face] == false) continue;
            FB_shadows.attach(gl::DEPTH_ATTACHMENT, data.light->tex, face);
            sq::CLEAR_DEPTH(); const Mat4F& matrix = data.light->matArr[face];

            renderer.pipeline.disable_stages(0, 0, 1);
            renderer.pipeline.use_shader(VS_shad_simple);
            for (const auto modelC : data.modelSimpleVecArr[face])
                draw_ModelSimple(matrix, modelC);
            for (const auto modelC : data.modelSimplePunchVecArr[face])
                draw_ModelSimple_punch(matrix, modelC);

            renderer.pipeline.disable_stages(0, 0, 1);
            renderer.pipeline.use_shader(VS_shad_skelly);
            for (const auto modelC : data.modelSkellyVecArr[face])
                draw_ModelSkelly(matrix, modelC);
            for (const auto modelC : data.modelSkellyPunchVecArr[face])
                draw_ModelSkelly_punch(matrix, modelC);
        }
    }

    sq::FRONTFACE(false);
}


void Shadows::update_settings() {

}


void Shadows::draw_ModelSimple(const Mat4F& _lightMat, const ModelComponent* _modelC) {
    VS_shad_simple.set_mat<Mat4F>("matrix", _lightMat * _modelC->matrix);
    sq::FRONTFACE(_modelC->negScale); _modelC->mesh->bind_vao();
    _modelC->mesh->draw_complete();
}

void Shadows::draw_ModelSimple_punch(const Mat4F& _lightMat, const ModelComponent* _modelC) {
    VS_shad_simple.set_mat<Mat4F>("matrix", _lightMat * _modelC->matrix);
    sq::FRONTFACE(_modelC->negScale); _modelC->mesh->bind_vao();
    for (uint i = 0u; i < _modelC->mesh->mtrlCount; ++i) {
        if (_modelC->skin->mtrlVec[i].punch == true)
            _modelC->skin->bind_textures(i, 1, 0, 0),
            renderer.pipeline.use_shader(FS_shad_punch);
        else renderer.pipeline.disable_stages(0, 0, 1);
        _modelC->mesh->draw_material(i);
    }
}

void Shadows::draw_ModelSkelly(const Mat4F& _lightMat, const ModelComponent* _modelC) {
    VS_shad_skelly.set_mat<Mat4F>("matrix", _lightMat * _modelC->matrix);
    sq::FRONTFACE(_modelC->negScale); _modelC->mesh->bind_vao();
    _modelC->ubo.bind(1u); _modelC->mesh->draw_complete();
}

void Shadows::draw_ModelSkelly_punch(const Mat4F& _lightMat, const ModelComponent* _modelC) {
    VS_shad_skelly.set_mat<Mat4F>("matrix", _lightMat * _modelC->matrix);
    sq::FRONTFACE(_modelC->negScale); _modelC->mesh->bind_vao(); _modelC->ubo.bind(1u);
    for (uint i = 0u; i < _modelC->mesh->mtrlCount; ++i) {
        if (_modelC->skin->mtrlVec[i].punch == true)
            _modelC->skin->bind_textures(i, 1, 0, 0),
            renderer.pipeline.use_shader(FS_shad_punch);
        else renderer.pipeline.disable_stages(0, 0, 1);
        _modelC->mesh->draw_material(i);
    }
}
