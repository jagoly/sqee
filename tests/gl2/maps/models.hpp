#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>

namespace sqt {

struct ModelInstance {
    void create(glm::vec3 _pos, uint _index, int _ambi, std::string& _mapPath);
    glm::vec3 pos;
    sq::Texture::Ptr texAmbi;
    uint index;
    glm::mat4 modelMat;
};

}
