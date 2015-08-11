#include <glm/gtc/matrix_transform.hpp>

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
    assert_fvec3  (_spec, name, "position");
    assert_fquat  (_spec, name, "rotation");
    assert_fvec3  (_spec, name, "scale");
    assert_string (_spec, name, "mesh");
    assert_string (_spec, name, "skin");

    PROP_shadow   = _spec.flagSet.count("shadow");
    PROP_render   = _spec.flagSet.count("render");
    PROP_decals   = _spec.flagSet.count("decals");
    PROP_position = _spec.fvec3Map.at("position");
    PROP_rotation = _spec.fquatMap.at("rotation");
    PROP_scale    = _spec.fvec3Map.at("scale");
    PROP_mesh     = _spec.stringMap.at("mesh");
    PROP_skin     = _spec.stringMap.at("skin");
}

void ModelStatic::refresh() {
    if (invalid == true) {
        if ((mesh = sq::res::mesh().get(PROP_mesh)) == nullptr)
            mesh = sq::res::mesh().add(PROP_mesh),
            mesh->create(PROP_mesh);

        if ((skin = sq::res::skin().get(PROP_skin)) == nullptr)
            skin = sq::res::skin().add(PROP_skin),
            skin->create(PROP_skin);

        invalid = false;
    }

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
