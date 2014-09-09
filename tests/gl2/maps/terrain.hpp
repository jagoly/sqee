#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/misc/files.hpp>

namespace sqt {


class Terrain {
public:
    int vCount;
    GLuint vao;
    sq::Texture::Ptr texNormArray, texDiffArray, texSpecArray;
    sq::Texture::Ptr texAmbi;

    bool load(glm::uvec2 _size, uint _texCount,
              const std::vector<std::string>& _texPaths,
              const std::string& _map,
              const int* _geometry, const bool* _smoothing,
              const int* _textures, const bool* _visible);
};

}
