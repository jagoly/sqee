#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/SkyLight.hpp"
#include "../wcoe/objects/SpotLight.hpp"
#include "../wcoe/objects/PointLight.hpp"
#include "../wcoe/objects/ModelSimple.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/Reflector.hpp"
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
        for (const wcoe::Reflector* rflct : data.reflectorVecArrA[csm])
            if (!rflct->skin->hasPunchThrough) draw_Reflector(matrix, *rflct);
        for (const wcoe::Reflector* rflct : data.reflectorVecArrA[csm])
            if (rflct->skin->hasPunchThrough) draw_Reflector_punch(matrix, *rflct);

        renderer.pipeline.disable_stages(0, 0, 1);
        for (const wcoe::ModelSimple* model : data.modelSimpleVecArrA[csm])
            if (!model->skin->hasPunchThrough) draw_ModelSimple(matrix, *model);
        for (const wcoe::ModelSimple* model : data.modelSimpleVecArrA[csm])
            if (model->skin->hasPunchThrough) draw_ModelSimple_punch(matrix, *model);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_skelly);
        for (const wcoe::ModelSkelly* model : data.modelSkellyVecArrA[csm])
            if (!model->skin->hasPunchThrough) draw_ModelSkelly(matrix, *model);
        for (const wcoe::ModelSkelly* model : data.modelSkellyVecArrA[csm])
            if (model->skin->hasPunchThrough) draw_ModelSkelly_punch(matrix, *model);
    }

    for (uint csm = 0u; csm < 2u; ++csm) {
        FB_shadows.attach(gl::DEPTH_ATTACHMENT, light.texB, csm);
        sq::CLEAR_DEPTH(); const Mat4F& matrix = light.matArrB[csm];

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_simple);
        for (const wcoe::Reflector* rflct : data.reflectorVecArrB[csm])
            if (!rflct->skin->hasPunchThrough) draw_Reflector(matrix, *rflct);
        for (const wcoe::Reflector* rflct : data.reflectorVecArrB[csm])
            if (rflct->skin->hasPunchThrough) draw_Reflector_punch(matrix, *rflct);

        renderer.pipeline.disable_stages(0, 0, 1);
        for (const wcoe::ModelSimple* model : data.modelSimpleVecArrB[csm])
            if (!model->skin->hasPunchThrough) draw_ModelSimple(matrix, *model);
        for (const wcoe::ModelSimple* model : data.modelSimpleVecArrB[csm])
            if (model->skin->hasPunchThrough) draw_ModelSimple_punch(matrix, *model);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_skelly);
        for (const wcoe::ModelSkelly* model : data.modelSkellyVecArrB[csm])
            if (!model->skin->hasPunchThrough) draw_ModelSkelly(matrix, *model);
        for (const wcoe::ModelSkelly* model : data.modelSkellyVecArrB[csm])
            if (model->skin->hasPunchThrough) draw_ModelSkelly_punch(matrix, *model);
    }

    sq::FRONTFACE(false);
    sq::DCLAMP_OFF();
}


void Shadows::render_shadows_spot() {
    for (const SpotLightData& data : renderer.spotLightDataVec) {
        FB_shadows.attach(gl::DEPTH_ATTACHMENT, data.light.tex);
        sq::CLEAR_DEPTH(); const Mat4F& matrix = data.light.matrix;
        sq::VIEWPORT(data.light.tex.get_size());

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_simple);
        for (const wcoe::Reflector* rflct : data.reflectorVec)
            if (!rflct->skin->hasPunchThrough) draw_Reflector(matrix, *rflct);
        for (const wcoe::Reflector* rflct : data.reflectorVec)
            if (rflct->skin->hasPunchThrough) draw_Reflector_punch(matrix, *rflct);

        renderer.pipeline.disable_stages(0, 0, 1);
        for (const wcoe::ModelSimple* model : data.modelSimpleVec)
            if (!model->skin->hasPunchThrough) draw_ModelSimple(matrix, *model);
        for (const wcoe::ModelSimple* model : data.modelSimpleVec)
            if (model->skin->hasPunchThrough) draw_ModelSimple_punch(matrix, *model);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(VS_shad_skelly);
        for (const wcoe::ModelSkelly* model : data.modelSkellyVec)
            if (!model->skin->hasPunchThrough) draw_ModelSkelly(matrix, *model);
        for (const wcoe::ModelSkelly* model : data.modelSkellyVec)
            if (model->skin->hasPunchThrough) draw_ModelSkelly_punch(matrix, *model);
    }

    sq::FRONTFACE(false);
}


void Shadows::render_shadows_point() {
    for (const PointLightData& data : renderer.pointLightDataVec) {
        sq::VIEWPORT(data.light.tex.get_size());

        for (uint face = 0u; face < 6u; ++face) {
            if (data.cullShadowFaceArr[face]) continue;
            FB_shadows.attach(gl::DEPTH_ATTACHMENT, data.light.tex, face);
            sq::CLEAR_DEPTH(); const Mat4F& matrix = data.light.matArr[face];

            renderer.pipeline.disable_stages(0, 0, 1);
            renderer.pipeline.use_shader(VS_shad_simple);
            for (const wcoe::Reflector* rflct : data.reflectorVecArr[face])
                if (!rflct->skin->hasPunchThrough) draw_Reflector(matrix, *rflct);
            for (const wcoe::Reflector* rflct : data.reflectorVecArr[face])
                if (rflct->skin->hasPunchThrough) draw_Reflector_punch(matrix, *rflct);

            renderer.pipeline.disable_stages(0, 0, 1);
            for (const wcoe::ModelSimple* model : data.modelSimpleVecArr[face])
                if (!model->skin->hasPunchThrough) draw_ModelSimple(matrix, *model);
            for (const wcoe::ModelSimple* model : data.modelSimpleVecArr[face])
                if (model->skin->hasPunchThrough) draw_ModelSimple_punch(matrix, *model);

            renderer.pipeline.disable_stages(0, 0, 1);
            renderer.pipeline.use_shader(VS_shad_skelly);
            for (const wcoe::ModelSkelly* model : data.modelSkellyVecArr[face])
                if (!model->skin->hasPunchThrough) draw_ModelSkelly(matrix, *model);
            for (const wcoe::ModelSkelly* model : data.modelSkellyVecArr[face])
                if (model->skin->hasPunchThrough) draw_ModelSkelly_punch(matrix, *model);
        }
    }

    sq::FRONTFACE(false);
}


void Shadows::update_settings() {

}


void Shadows::draw_Reflector(const Mat4F& _lightMat, const wcoe::Reflector& _rflct) {
    VS_shad_simple.set_mat<Mat4F>("matrix", _lightMat * _rflct.matrix);
    sq::FRONTFACE(_rflct.negScale); _rflct.mesh->bind_vao();
    _rflct.mesh->draw_complete();
}

void Shadows::draw_Reflector_punch(const Mat4F& _lightMat, const wcoe::Reflector& _rflct) {
    VS_shad_simple.set_mat<Mat4F>("matrix", _lightMat * _rflct.matrix);
    sq::FRONTFACE(_rflct.negScale); _rflct.mesh->bind_vao();
    for (uint i = 0u; i < _rflct.mesh->mtrlCount; ++i) {
        if (_rflct.skin->mtrlVec[i].punch == true)
            _rflct.skin->bind_textures(i, 1, 0, 0),
            renderer.pipeline.use_shader(FS_shad_punch);
        else renderer.pipeline.disable_stages(0, 0, 1);
        _rflct.mesh->draw_material(i);
    }
}

void Shadows::draw_ModelSimple(const Mat4F& _lightMat, const wcoe::ModelSimple& _model) {
    VS_shad_simple.set_mat<Mat4F>("matrix", _lightMat * _model.matrix);
    sq::FRONTFACE(_model.negScale); _model.mesh->bind_vao();
    _model.mesh->draw_complete();
}

void Shadows::draw_ModelSimple_punch(const Mat4F& _lightMat, const wcoe::ModelSimple& _model) {
    VS_shad_simple.set_mat<Mat4F>("matrix", _lightMat * _model.matrix);
    sq::FRONTFACE(_model.negScale); _model.mesh->bind_vao();
    for (uint i = 0u; i < _model.mesh->mtrlCount; ++i) {
        if (_model.skin->mtrlVec[i].punch == true)
            _model.skin->bind_textures(i, 1, 0, 0),
            renderer.pipeline.use_shader(FS_shad_punch);
        else renderer.pipeline.disable_stages(0, 0, 1);
        _model.mesh->draw_material(i);
    }
}

void Shadows::draw_ModelSkelly(const Mat4F& _lightMat, const wcoe::ModelSkelly& _model) {
    VS_shad_skelly.set_mat<Mat4F>("matrix", _lightMat * _model.matrix);
    sq::FRONTFACE(_model.negScale); _model.mesh->bind_vao();
    _model.ubo.bind(1u); _model.mesh->draw_complete();
}

void Shadows::draw_ModelSkelly_punch(const Mat4F& _lightMat, const wcoe::ModelSkelly& _model) {
    VS_shad_skelly.set_mat<Mat4F>("matrix", _lightMat * _model.matrix);
    sq::FRONTFACE(_model.negScale); _model.mesh->bind_vao(); _model.ubo.bind(1u);
    for (uint i = 0u; i < _model.mesh->mtrlCount; ++i) {
        if (_model.skin->mtrlVec[i].punch == true)
            _model.skin->bind_textures(i, 1, 0, 0),
            renderer.pipeline.use_shader(FS_shad_punch);
        else renderer.pipeline.disable_stages(0, 0, 1);
        _model.mesh->draw_material(i);
    }
}
