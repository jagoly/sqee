#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include "../cell.hpp"
#include "modelstatic.hpp"

using namespace sqt::wcoe;

ModelStatic::ModelStatic(const string& _name, const Cell* _cell)
    : Object(ObjType::ModelStatic, _name, _cell) {}

void ModelStatic::load_from_spec(const ObjSpec& _spec) {
    DAT_shadow = _spec.flags.count("shadow");
    DAT_render = _spec.flags.count("render");
    DAT_reflect = _spec.flags.count("reflect");
    DAT_refract = _spec.flags.count("refract");
    DAT_mPath = _spec.sMap.at("mesh")[0];
    DAT_sPath = _spec.sMap.at("skin")[0];

    if (_spec.fMap.count("pos")) DAT_pos = glm::make_vec3(_spec.fMap.at("pos").data());
    if (_spec.fMap.count("rot")) DAT_rot = glm::make_vec3(_spec.fMap.at("rot").data());
    if (_spec.fMap.count("sca")) DAT_sca = glm::make_vec3(_spec.fMap.at("sca").data());
}

void ModelStatic::update_from_data() {
    if (!(mesh = sq::res::mesh().get(DAT_mPath)))
        mesh = sq::res::mesh().add(DAT_mPath),
        mesh->create(DAT_mPath);

    if (!(skin = sq::res::skin().get(DAT_sPath)))
        skin = sq::res::skin().add(DAT_sPath),
        skin->create(DAT_sPath);

    matrix = glm::translate(mat4(), DAT_pos + cell->position);
    matrix = glm::rotate(matrix, glm::radians(DAT_rot.x), {1,0,0});
    matrix = glm::rotate(matrix, glm::radians(DAT_rot.y), {0,1,0});
    matrix = glm::rotate(matrix, glm::radians(DAT_rot.z), {0,0,1});
    matrix = glm::scale(matrix, DAT_sca);
    negScale = glm::determinant(matrix) < 0.f;
    bbox = sq::make_BoundBox(matrix, mesh->origin, mesh->size, mesh->radius);
}
