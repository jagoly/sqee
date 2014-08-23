#include <libsqee/gl/gl.hpp>

namespace sqt {

struct ModelInstance {
    ModelInstance(glm::vec3 _pos, int _index);
    glm::vec3 pos;
    int index;
    glm::mat4 modelMat;
};

}
