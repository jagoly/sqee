#include "models.hpp"

using namespace sqt;

ModelInstance::ModelInstance(glm::vec3 _pos, int _index) {
    pos = _pos;
    index = _index;
    modelMat = glm::translate(glm::mat4(), pos);
}
