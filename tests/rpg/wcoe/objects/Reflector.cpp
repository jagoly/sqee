#include <sqee/app/Resources.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "Reflector.hpp"

using namespace sqt::wcoe;
namespace maths = sq::maths;

Reflector::Reflector(const string& _name, Cell* _cell)
    : Object(typeid(Reflector), _name, _cell) {
    ubo.reserve("matrix", 16u);
    ubo.reserve("normMat", 16u);
    ubo.reserve("normal", 4u);
    ubo.reserve("trans", 3u);
    ubo.reserve("factor", 1u);
    ubo.allocate_storage();
}

void Reflector::load_from_spec(const ObjSpec& _spec) {
    _spec.set_if("shadow", PROP_shadow);
    _spec.set_if("decals", PROP_decals);
    _spec.set_if("probes", PROP_probes);
    _spec.set_if("position", PROP_position);
    _spec.set_if("rotation", PROP_rotation);
    _spec.set_if("scale", PROP_scale);
    _spec.set_if("factor", PROP_factor);
    _spec.set_if("mesh", PROP_mesh);
    _spec.set_if("skin", PROP_skin);
}

void Reflector::refresh() {
    if (invalid == false) return;

    if ((mesh = sq::static_Mesh().get(PROP_mesh)) == nullptr)
        mesh = sq::static_Mesh().add(PROP_mesh, PROP_mesh);

    if ((skin = sq::static_Skin().get(PROP_skin)) == nullptr)
        skin = sq::static_Skin().add(PROP_skin, PROP_skin);

    animate(); invalid = false;
}

void Reflector::update() {
    bool doFinish = false;
    if (ANIM_position.active()) if (ANIM_position.tick()) doFinish = true;
    if (ANIM_rotation.active()) if (ANIM_rotation.tick()) doFinish = true;
    if (ANIM_scale.active())    if (ANIM_scale.tick())    doFinish = true;
    if (ANIM_factor.active())   if (ANIM_factor.tick())   doFinish = true;
    if (doFinish == true) animate();
}

void Reflector::calc(double _accum) {
    bool doAnim = false;
    if (ANIM_position.active()) { ANIM_position.calc(_accum); doAnim = true; }
    if (ANIM_rotation.active()) { ANIM_rotation.calc(_accum); doAnim = true; }
    if (ANIM_scale.active())    { ANIM_scale.calc(_accum);    doAnim = true; }
    if (ANIM_factor.active())   { ANIM_factor.calc(_accum);   doAnim = true; }
    if (doAnim == true) animate();

    frus = sq::reflect_Frustum(world.camera->frus, normal, trans);
    Mat4F normMat(sq::make_normMat(world.camera->viewMat * matrix));
    ubo.update("normMat", &normMat);
}

void Reflector::animate() {
    matrix = maths::translate(Mat4F(), PROP_position + cell->PROP_position);
    matrix *= Mat4F(PROP_rotation); matrix = maths::scale(matrix, PROP_scale);
    bbox = sq::make_BoundBox(matrix, mesh->origin, mesh->radius, mesh->bbsize);
    normal = maths::normalize(sq::make_normMat(matrix) * Vec3F(0.f, 0.f, 1.f));

    trans = PROP_position + cell->PROP_position;
    negScale = maths::determinant(matrix) < 0.f;
    offset = maths::dot(-normal, trans);

    ubo.update("matrix", &matrix);
    ubo.update("normal", &normal);
    ubo.update("trans", &trans);
    ubo.update("factor", &PROP_factor);
}
