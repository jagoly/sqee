#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/models/modelstatic.hpp>
#include <sqee/gl/maths.hpp>

#include "../cell.hpp"
#include "model.hpp"

using namespace sqt;

Model::Model(const ObjSpec& _spec) : Object(_spec) {
    refl = _spec.flagSet.count("refl");
    refr = _spec.flagSet.count("refr");

    model.reset(new sq::ModelStatic());

    const string& mPath = _spec.sMap.at("mesh")[0];
    if (!(model->mesh = sq::res::mesh().get(mPath))) {
        model->mesh = sq::res::mesh().add(mPath);
        model->mesh->create(mPath);
    }

    const string& sPath = _spec.sMap.at("skin")[0];
    if (!(model->skin = sq::res::skin().get(sPath))) {
        model->skin = sq::res::skin().add(sPath);
        model->skin->create(sPath);
    }

    vec3 pos(0,0,0), rot(0,0,0), sca(1,1,1);
    if (_spec.fMap.count("pos"))
        pos = glm::make_vec3(_spec.fMap.at("pos").data());
    if (_spec.fMap.count("rot"))
        rot = glm::make_vec3(_spec.fMap.at("rot").data());
    if (_spec.fMap.count("sca"))
        sca = glm::make_vec3(_spec.fMap.at("sca").data());

    model->matrix = glm::translate(mat4(), pos);
    model->matrix = glm::translate(model->matrix, cell.pos);
    model->matrix = glm::rotate(model->matrix, glm::radians(rot.x), {1,0,0});
    model->matrix = glm::rotate(model->matrix, glm::radians(rot.y), {0,1,0});
    model->matrix = glm::rotate(model->matrix, glm::radians(rot.z), {0,0,1});
    model->matrix = glm::scale(model->matrix, sca);
    model->bbox = sq::bbox_by_model(model->mesh->bbox, model->matrix);
}
