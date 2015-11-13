#include <sqee/app/Resources.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "ModelSimple.hpp"

using namespace sqt::wcoe;
namespace maths = sq::maths;

ModelSimple::ModelSimple(const string& _name, Cell* _cell)
    : Object(typeid(ModelSimple), _name, _cell) {

    ubo.reserve("matrix", 16u);
    ubo.reserve("normMat", 16u);
    ubo.allocate_storage();
}

void ModelSimple::load_from_spec(const ObjSpec& _spec) {
    _spec.set_if("shadow", PROP_shadow);
    _spec.set_if("render", PROP_render);
    _spec.set_if("decals", PROP_decals);
    _spec.set_if("probes", PROP_probes);
    _spec.set_if("position", PROP_position);
    _spec.set_if("rotation", PROP_rotation);
    _spec.set_if("scale", PROP_scale);
    _spec.set_if("mesh", PROP_mesh);
    _spec.set_if("skin", PROP_skin);
}

void ModelSimple::refresh() {
    if (invalid == false) return;

    if ((mesh = sq::static_Mesh().get(PROP_mesh)) == nullptr)
        mesh = sq::static_Mesh().add(PROP_mesh, PROP_mesh);

    if ((skin = sq::static_Skin().get(PROP_skin)) == nullptr)
        skin = sq::static_Skin().add(PROP_skin, PROP_skin);

    animate(); invalid = false;
}

void ModelSimple::update() {
    bool doFinish = false;
    if (ANIM_position.active()) if (ANIM_position.tick()) doFinish = true;
    if (ANIM_rotation.active()) if (ANIM_rotation.tick()) doFinish = true;
    if (ANIM_scale.active())    if (ANIM_scale.tick())    doFinish = true;
    if (doFinish == true) animate();
}

void ModelSimple::calc(double _accum) {
    bool doAnim = false;
    if (ANIM_position.active()) { ANIM_position.calc(_accum); doAnim = true; }
    if (ANIM_rotation.active()) { ANIM_rotation.calc(_accum); doAnim = true; }
    if (ANIM_scale.active())    { ANIM_scale.calc(_accum);    doAnim = true; }
    if (doAnim == true) animate();

    Mat4F normMat(sq::make_normMat(world.camera->viewMat * matrix));
    ubo.update("normMat", &normMat);
}

void ModelSimple::animate() {
    matrix = maths::translate(Mat4F(), PROP_position + cell->PROP_position);
    matrix *= Mat4F(PROP_rotation); matrix = maths::scale(matrix, PROP_scale);
    bbox = sq::make_BoundBox(matrix, mesh->origin, mesh->radius, mesh->bbsize);
    negScale = maths::determinant(matrix) < 0.f;
    ubo.update("matrix", &matrix);
}
