#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>

namespace sqt {

struct ModelInstance {
    ModelInstance(glm::vec3 _pos, int _i1, int _i2, const std::string& _mapPath);
    glm::vec3 pos;
    sq::Texture2D texAmbi;
    int index;
    glm::mat4 modelMat;
};

}
