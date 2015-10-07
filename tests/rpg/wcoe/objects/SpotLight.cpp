#include <glm/gtc/matrix_transform.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "SpotLight.hpp"

using namespace sqt::wcoe;

SpotLight::SpotLight(const string& _name, Cell* _cell) : Object(_name, _cell) {
    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("direction", 3);
    ubo->reserve("intensity", 1);
    ubo->reserve("position", 3);
    ubo->reserve("softness", 1);
    ubo->reserve("colour", 3);
    ubo->reserve("angle", 1);
    ubo->reserve("matrix", 16);
    ubo->reserve("modelMat", 16);
    ubo->allocate_storage();
}

void SpotLight::load_from_spec(const ObjSpec& _spec) {
    _spec.set_if("shadow", PROP_shadow);
    _spec.set_if("specular", PROP_specular);
    _spec.set_if("probes", PROP_probes);
    _spec.set_if("direction", PROP_direction);
    _spec.set_if("position", PROP_position);
    _spec.set_if("colour", PROP_colour);
    _spec.set_if("intensity", PROP_intensity);
    _spec.set_if("softness", PROP_softness);
    _spec.set_if("angle", PROP_angle);
    _spec.set_if("texsize", PROP_texsize);
}

void SpotLight::refresh() {
    bool recreateTexture = false;

    if (check_invalid()) {
        if (PROP_shadow == true)
            recreateTexture = true;
        else tex.reset();
    }

    if (PROP_shadow && settings.check<bool>("rpg_shadlarge"))
        recreateTexture = true;

    if (recreateTexture == true) {
        tex.reset(new sq::Texture2D());
        bool shadlarge = settings.get<bool>("rpg_shadlarge");
        tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1u, false);
        tex->allocate_storage(uvec2(PROP_texsize * (1u + shadlarge)));
        tex->set_preset(sq::Texture::ShadowMap());
    }

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

    ubo->update("direction", &PROP_direction);
    ubo->update("intensity", &PROP_intensity);
    ubo->update("position", &position);
    ubo->update("softness", &PROP_softness);
    ubo->update("colour", &PROP_colour);
    ubo->update("angle", &angle);
    ubo->update("matrix", &matrix);
    ubo->update("modelMat", &modelMat);
}
