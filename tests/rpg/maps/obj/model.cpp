#include <fstream>

#include "../../resbank.hpp"
#include "model.hpp"

using namespace sqt;

Model::Model(const ObjectSpec& _spec) : Object(_spec) {
    shad  = _spec.flagSet.count("shad");
    pnch  = _spec.flagSet.count("pnch");
    refl  = _spec.flagSet.count("refl");
    refr  = _spec.flagSet.count("refr");

    const string& mName = _spec.sMap.at("mesh")[0];
    if (!(mesh = resBank().meshH.get(mName))) {
        mesh = resBank().meshH.add(mName);
        mesh->load(mName);
    }

    skin.load(_spec.sMap.at("skin")[0], resBank().texH);

    glm::vec3 pos = {0, 0, 0};
    glm::vec3 rot = {0, 0, 0};
    glm::vec3 sca = {1, 1, 1};
    if (_spec.fMap.count("pos"))
        pos = glm::make_vec3(_spec.fMap.at("pos").data());
    if (_spec.fMap.count("rot"))
        rot = glm::make_vec3(_spec.fMap.at("rot").data());
    if (_spec.fMap.count("sca"))
        sca = glm::make_vec3(_spec.fMap.at("sca").data());

    modelMat = sq::iMat4;
    modelMat = glm::translate(sq::iMat4, pos);
    modelMat = glm::translate(modelMat, cellPos);
    modelMat = glm::rotate(modelMat, rot.x, {1, 0, 0});
    modelMat = glm::rotate(modelMat, rot.y, {0, 1, 0});
    modelMat = glm::rotate(modelMat, rot.z, {0, 0, 1});
    modelMat = glm::scale(modelMat, sca);
}
