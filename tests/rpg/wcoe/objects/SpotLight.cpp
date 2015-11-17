#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/app/Settings.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "SpotLight.hpp"

using namespace sqt::wcoe;
namespace maths = sq::maths;

SpotLight::SpotLight(const string& _name, Cell* _cell)
    : Object(typeid(SpotLight), _name, _cell),
    tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()) {

    ubo.reserve("direction", 3u);
    ubo.reserve("intensity", 1u);
    ubo.reserve("position", 3u);
    ubo.reserve("softness", 1u);
    ubo.reserve("colour", 3u);
    ubo.reserve("angle", 1u);
    ubo.reserve("matrix", 16u);
    ubo.allocate_storage();
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
    if (invalid == false) return;

    if (PROP_shadow == true) {
        uint sizeMult = settings.get<bool>("rpg_shadlarge") + 1u;
        tex.allocate_storage(Vec2U(PROP_texsize * sizeMult), false);
    } else tex.delete_object();

    animate(); invalid = false;
}

void SpotLight::update() {
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
    Vec3F position = PROP_position + cell->PROP_position;
    Vec3F tangent = sq::make_tangent(PROP_direction);
    float angle = maths::radians(PROP_angle);

    Mat4F viewMat = maths::look_at(position, position+PROP_direction, tangent);
    matrix = maths::perspective(2.f*angle, 1.f, 0.2f, PROP_intensity) * viewMat;
    frus = sq::make_Frustum(matrix, position, PROP_direction, 0.2f, PROP_intensity);

    float tanAngle = -std::tan(angle*2.f);
    Vec3F scale = Vec3F(tanAngle, tanAngle, -1.f) * PROP_intensity;
    modelMat = maths::inverse(viewMat) * maths::scale(Mat4F(), scale);

    ubo.update("direction", &PROP_direction);
    ubo.update("intensity", &PROP_intensity);
    ubo.update("position", &position);
    ubo.update("softness", &PROP_softness);
    ubo.update("colour", &PROP_colour);
    ubo.update("angle", &angle);
    ubo.update("matrix", &matrix);
}
