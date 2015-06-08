#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/app/logging.hpp>
#include <glm/gtx/string_cast.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include "../../main/camera.hpp"
#include "../cell.hpp"
#include "../world.hpp"
#include "reflector.hpp"

using namespace sqt::wcoe;

Reflector::Reflector(const string& _name, Cell* _cell)
    : Object(ObjType::Reflector, _name, _cell) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("matrix", 16);
    ubo->reserve("normMat", 16);
    ubo->reserve("normal", 4);
    ubo->reserve("trans", 3);
    ubo->reserve("factor", 1);
    ubo->create();
}

void Reflector::load_from_spec(const ObjSpec& _spec) {
    SPEC_ASSERT_FLOAT("position", 3);
    SPEC_ASSERT_FLOAT("rotation", 4);
    SPEC_ASSERT_FLOAT("scale", 3);
    SPEC_ASSERT_FLOAT("factor", 1);
    SPEC_ASSERT_STRING("mesh", 1);
    SPEC_ASSERT_STRING("skin", 1);

    PROP_shadow   = SPEC_HAS_FLAG("shadow");
    PROP_position = glm::make_vec3(_spec.fMap.at("position").data());
    PROP_rotation = glm::make_quat(_spec.fMap.at("rotation").data());
    PROP_scale    = glm::make_vec3(_spec.fMap.at("scale").data());
    PROP_factor   = _spec.fMap.at("factor")[0];
    PROP_mPath    = _spec.sMap.at("mesh")[0];
    PROP_sPath    = _spec.sMap.at("skin")[0];
}

void Reflector::refresh() {
    if (!(mesh = sq::res::mesh().get(PROP_mPath)))
        mesh = sq::res::mesh().add(PROP_mPath),
        mesh->create(PROP_mPath);

    if (!(skin = sq::res::skin().get(PROP_sPath)))
        skin = sq::res::skin().add(PROP_sPath),
        skin->create(PROP_sPath);

    animate();
}

void Reflector::tick() {
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
    if (doAnim == true) animate(); else ubo->bind(2);

    frus = sq::reflect_Frustum(cell->world->camera->frus, normal, trans);
    fmat4 normMat(sq::make_normMat(cell->world->camera->viewMat * matrix));
    ubo->update("normMat", &normMat);
}

void Reflector::animate() {
    trans = PROP_position + cell->DAT_position;
    matrix = glm::translate(fmat4(), trans);
    matrix *= glm::mat4_cast(PROP_rotation);
    matrix = glm::scale(matrix, PROP_scale);
    negScale = glm::determinant(matrix) < 0.f;
    normal = glm::normalize(sq::make_normMat(matrix) * fvec3(0,0,1));
    bbox = sq::make_BoundBox(matrix, mesh->origin, mesh->size, mesh->radius);

    ubo->bind(2);
    ubo->update("matrix", &matrix);
    ubo->update("normal", &normal);
    ubo->update("trans", &trans);
    ubo->update("factor", &PROP_factor);
}
