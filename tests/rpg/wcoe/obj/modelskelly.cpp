#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/render/skeleton.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/gl/maths.hpp>

#include "../cell.hpp"
#include "modelskelly.hpp"

using namespace sqt::wcoe;

ModelSkelly::ModelSkelly(const string& _name, const Cell& _cell)
    : Object(ObjType::ModelSkelly, _name, _cell) {
    skel.reset(new sq::Skeleton());
}

void ModelSkelly::load_from_spec(const ObjSpec& _spec) {
    refl = _spec.flags.count("refl");
    refr = _spec.flags.count("refr");

    const string& mPath = _spec.sMap.at("mesh")[0];
    if (!(mesh = sq::res::mesh().get(mPath))) {
        mesh = sq::res::mesh().add(mPath);
        mesh->create(mPath);
    }

    const string& sPath = _spec.sMap.at("skin")[0];
    if (!(skin = sq::res::skin().get(sPath))) {
        skin = sq::res::skin().add(sPath);
        skin->create(sPath);
    }

    vec3 pos(0,0,0), rot(0,0,0), sca(1,1,1);
    if (_spec.fMap.count("pos"))
        pos = glm::make_vec3(_spec.fMap.at("pos").data());
    if (_spec.fMap.count("rot"))
        rot = glm::make_vec3(_spec.fMap.at("rot").data());
    if (_spec.fMap.count("sca"))
        sca = glm::make_vec3(_spec.fMap.at("sca").data());

    matrix = glm::translate(mat4(), pos + cell.position);
    matrix = glm::rotate(matrix, glm::radians(rot.x), {1,0,0});
    matrix = glm::rotate(matrix, glm::radians(rot.y), {0,1,0});
    matrix = glm::rotate(matrix, glm::radians(rot.z), {0,0,1});
    matrix = glm::scale(matrix, sca);
    bbox = sq::bbox_by_model(mesh->bbox, matrix);
    negScale = glm::determinant(matrix) < 0.f;
}
