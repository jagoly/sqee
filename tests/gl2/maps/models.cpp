#include <iostream>

#include "models.hpp"

using namespace sqt;

const unsigned char whitePixel[4] = {255, 255, 255, 255};
const unsigned char blackPixel[4] = {0, 0, 0, 255};

ModelInstance::ModelInstance(glm::vec3 _pos, int _i1, int _i2, const std::string& _mapPath) {
    pos = _pos;
    index = _i1;
    modelMat = glm::translate(glm::mat4(), pos);
    if      (_i2 == -1) texAmbi.load_from_memory({1, 1}, gl::R16F, blackPixel);
    else if (_i2 == -2) texAmbi.load_from_memory({1, 1}, gl::R16F, whitePixel);
    else                texAmbi.load_from_file("res/maps/" + _mapPath + "/bakes/" +
                        std::to_string(_i1)+"_"+std::to_string(_i2)+".png", gl::R16F);

    texAmbi.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
    texAmbi.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
}
