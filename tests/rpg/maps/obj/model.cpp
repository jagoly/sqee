#include <fstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/models/mesh.hpp>
#include <sqee/models/skin.hpp>

#include "../cell.hpp"
#include "model.hpp"

using namespace sqt;

Model::Model(const ObjSpec& _spec) : Object(_spec) {
    shad = _spec.flagSet.count("shad");
    pnch = _spec.flagSet.count("pnch");
    refl = _spec.flagSet.count("refl");
    refr = _spec.flagSet.count("refr");

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

    glm::vec3 pos = {0, 0, 0};
    glm::vec3 rot = {0, 0, 0};
    glm::vec3 sca = {1, 1, 1};
    if (_spec.fMap.count("pos"))
        pos = glm::make_vec3(_spec.fMap.at("pos").data());
    if (_spec.fMap.count("rot"))
        rot = glm::make_vec3(_spec.fMap.at("rot").data());
    if (_spec.fMap.count("sca"))
        sca = glm::make_vec3(_spec.fMap.at("sca").data());

    modelMat = glm::translate(glm::mat4(), pos);
    modelMat = glm::translate(modelMat, cell.pos);
    modelMat = glm::rotate(modelMat, rot.x, {1, 0, 0});
    modelMat = glm::rotate(modelMat, rot.y, {0, 1, 0});
    modelMat = glm::rotate(modelMat, rot.z, {0, 0, 1});
    modelMat = glm::scale(modelMat, sca);
}
