#include <glm/gtc/matrix_transform.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "ModelSimple.hpp"

using namespace sqt::wcoe;

ModelSimple::ModelSimple(const string& _name, Cell* _cell) : Object(_name, _cell) {
    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("matrix", 16);
    ubo->reserve("normMat", 16);
    ubo->allocate_storage();
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
    if (check_invalid() == true) {
        if ((mesh = sq::res::mesh().get(PROP_mesh)) == nullptr)
            mesh = sq::res::mesh().add(PROP_mesh),
            mesh->create(PROP_mesh);

        if ((skin = sq::res::skin().get(PROP_skin)) == nullptr)
            skin = sq::res::skin().add(PROP_skin),
            skin->create(PROP_skin);
    }

    animate();
}

void ModelSimple::tick() {
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

    fmat4 normMat(sq::make_normMat(cell->world->camera->viewMat * matrix));
    ubo->update("normMat", &normMat);
}

void ModelSimple::animate() {
    matrix = glm::translate(fmat4(), PROP_position + cell->DAT_position);
    matrix *= glm::mat4_cast(PROP_rotation); matrix = glm::scale(matrix, PROP_scale);
    bbox = sq::make_BoundBox(matrix, mesh->origin, mesh->radius, mesh->bbsize);
    negScale = glm::determinant(matrix) < 0.f;
    ubo->update("matrix", &matrix);
}
