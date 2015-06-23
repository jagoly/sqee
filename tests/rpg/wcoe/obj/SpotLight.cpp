#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "SpotLight.hpp"

using namespace sqt::wcoe;

SpotLight::SpotLight(const string& _name, Cell* _cell)
    : Object(ObjType::SpotLight, _name, _cell) {
    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("direction", 3);
    ubo->reserve("intensity", 1);
    ubo->reserve("position", 3);
    ubo->reserve("softness", 1);
    ubo->reserve("colour", 3);
    ubo->reserve("angle", 1);
    ubo->reserve("matrix", 16);
    ubo->reserve("modelMat", 16);
    ubo->create();
}

void SpotLight::load_from_spec(const ObjSpec& _spec) {
    SPEC_ASSERT_FLOAT("direction", 3);
    SPEC_ASSERT_FLOAT("position", 3);
    SPEC_ASSERT_FLOAT("colour", 3);
    SPEC_ASSERT_FLOAT("intensity", 1);
    SPEC_ASSERT_FLOAT("softness", 1);
    SPEC_ASSERT_FLOAT("angle", 1);

    PROP_shadow    = SPEC_HAS_FLAG("shadow");
    PROP_specular  = SPEC_HAS_FLAG("specular");
    PROP_direction = glm::make_vec3(_spec.fMap.at("direction").data());
    PROP_position  = glm::make_vec3(_spec.fMap.at("position").data());
    PROP_colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    PROP_intensity = _spec.fMap.at("intensity")[0];
    PROP_softness  = _spec.fMap.at("softness")[0];
    PROP_angle     = _spec.fMap.at("angle")[0];
    if (PROP_shadow) PROP_texsize = _spec.iMap.at("texsize")[0];
}

void SpotLight::refresh() {
    if (PROP_shadow == true) {
        tex.reset(new sq::Texture2D());
        tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        fbo.reset(new sq::FrameBuffer()); fbo->attach(gl::DEPTH_ATTACHMENT, *tex);
    } else { tex.reset(); fbo.reset(); }

    animate();
}

void SpotLight::tick() {
    bool doFinish = false;
    if (ANIM_direction.active()) if (ANIM_direction.tick()) doFinish = true;
    if (ANIM_position.active())  if (ANIM_position.tick())  doFinish = true;
    if (ANIM_colour.active())    if (ANIM_colour.tick())    doFinish = true;
    if (ANIM_intensity.active()) if (ANIM_intensity.tick()) doFinish = true;
    if (ANIM_softness.active())  if (ANIM_softness.tick())  doFinish = true;
    if (ANIM_angle.active())     if (ANIM_angle.tick())     doFinish = true;
    if (doFinish == true) animate();
}

void SpotLight::calc(double _accum) {
    bool doAnim = false;
    if (ANIM_direction.active()) { ANIM_direction.calc(_accum); doAnim = true; }
    if (ANIM_position.active())  { ANIM_position.calc(_accum);  doAnim = true; }
    if (ANIM_colour.active())    { ANIM_colour.calc(_accum);    doAnim = true; }
    if (ANIM_intensity.active()) { ANIM_intensity.calc(_accum); doAnim = true; }
    if (ANIM_softness.active())  { ANIM_softness.calc(_accum);  doAnim = true; }
    if (ANIM_angle.active())     { ANIM_angle.calc(_accum);     doAnim = true; }
    if (doAnim == true) animate();
}

void SpotLight::animate() {
    fvec3 position = PROP_position + cell->DAT_position;
    fvec3 tangent = sq::make_tangent(PROP_direction);
    float angle = glm::radians(PROP_angle);

    fmat4 viewMat = glm::lookAt(position, position+PROP_direction, tangent);
    matrix = glm::perspective(2.f*angle, 1.f, 0.2f, PROP_intensity) * viewMat;
    frus = sq::make_Frustum(matrix, position, PROP_direction, 0.2, PROP_intensity);

    float tanAngle = glm::tan(angle*2.f);
    fvec3 scale = -fvec3(tanAngle, tanAngle, 1.f) * PROP_intensity;
    modelMat = glm::inverse(viewMat) * glm::scale(fmat4(), scale);

    ubo->bind(1);
    ubo->update("direction", &PROP_direction);
    ubo->update("intensity", &PROP_intensity);
    ubo->update("position", &position);
    ubo->update("softness", &PROP_softness);
    ubo->update("colour", &PROP_colour);
    ubo->update("angle", &angle);
    ubo->update("matrix", &matrix);
    ubo->update("modelMat", &modelMat);
}
