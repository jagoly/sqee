#include <iostream>

#include "models.hpp"

using namespace sqt;

const unsigned char whitePixel[4] = {255, 255, 255, 255};
const unsigned char blackPixel[4] = {0, 0, 0, 255};

void ModelInstance::create(glm::vec3 _pos, uint _index, int _ambi, std::string& _mapPath) {
    pos = _pos;
    index = _index;
    modelMat = glm::translate(glm::mat4(), pos);
    if      (_ambi == -1) texAmbi = sq::tex2D_load_memory({1, 1}, gl::R16F, blackPixel);
    else if (_ambi == -2) texAmbi = sq::tex2D_load_memory({1, 1}, gl::R16F, whitePixel);
    else                  texAmbi = sq::tex2D_load_file("res/maps/" + _mapPath + "/bakes/" +
                          std::to_string(_index)+"_"+std::to_string(_ambi)+".png", gl::R16F);

    if (texAmbi == nullptr) texAmbi = sq::tex2D_load_blank({1, 1}, gl::R16F);

    texAmbi->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
    texAmbi->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
}
