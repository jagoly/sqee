#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "ModelStatic.hpp"

using namespace sqt::wcoe;

ModelStatic::ModelStatic(const string& _name, Cell* _cell)
    : Object(ObjType::ModelStatic, _name, _cell) {
    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("matrix", 16);
    ubo->reserve("normMat", 16);
    ubo->create();
}

void ModelStatic::load_from_spec(const ObjSpec& _spec) {
    SPEC_ASSERT_FLOAT("position", 3);
    SPEC_ASSERT_FLOAT("rotation", 4);
    SPEC_ASSERT_FLOAT("scale", 3);
    SPEC_ASSERT_STRING("mesh", 1);
    SPEC_ASSERT_STRING("skin", 1);

    PROP_shadow   = SPEC_HAS_FLAG("shadow");
    PROP_render   = SPEC_HAS_FLAG("render");
    PROP_position = glm::make_vec3(_spec.fMap.at("position").data());
    PROP_rotation = glm::make_quat(_spec.fMap.at("rotation").data());
    PROP_scale    = glm::make_vec3(_spec.fMap.at("scale").data());
    PROP_meshPath = _spec.sMap.at("mesh")[0];
    PROP_skinPath = _spec.sMap.at("skin")[0];
}

void ModelStatic::refresh() {
    if ((mesh = sq::res::mesh().get(PROP_meshPath)) == nullptr)
        mesh = sq::res::mesh().add(PROP_meshPath),
        mesh->create(PROP_meshPath);

    if ((skin = sq::res::skin().get(PROP_skinPath)) == nullptr)
        skin = sq::res::skin().add(PROP_skinPath),
        skin->create(PROP_skinPath);

    animate();
}

void ModelStatic::tick() {
    bool doFinish = false;
    if (ANIM_position.active()) if (ANIM_position.tick()) doFinish = true;
    if (ANIM_rotation.active()) if (ANIM_rotation.tick()) doFinish = true;
    if (ANIM_scale.active())    if (ANIM_scale.tick())    doFinish = true;
    if (doFinish == true) animate();
}

void ModelStatic::calc(double _accum) {
    bool doAnim = false;
    if (ANIM_position.active()) { ANIM_position.calc(_accum); doAnim = true; }
    if (ANIM_rotation.active()) { ANIM_rotation.calc(_accum); doAnim = true; }
    if (ANIM_scale.active())    { ANIM_scale.calc(_accum);    doAnim = true; }
    if (doAnim == true) animate(); else ubo->bind(1);

    fmat4 normMat(sq::make_normMat(cell->world->camera->viewMat * matrix));
    ubo->update("normMat", &normMat);
}

void ModelStatic::animate() {
    matrix = glm::translate(fmat4(), PROP_position + cell->DAT_position);
    matrix *= glm::mat4_cast(PROP_rotation); matrix = glm::scale(matrix, PROP_scale);
    bbox = sq::make_BoundBox(matrix, mesh->origin, mesh->radius, mesh->bbsize);
    ubo->bind(1); ubo->update("matrix", &matrix);
    negScale = glm::determinant(matrix) < 0.f;
}
