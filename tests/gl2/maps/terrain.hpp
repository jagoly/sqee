#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/misc/files.hpp>

namespace sqt {


class Terrain {
public:
    int vCount;
    GLuint vao;
    sq::Texture2DArray texNormArray, texDiffArray, texSpecArray;

    bool load(glm::uvec2 _size, uint _texCount,std::vector<std::string> _texPaths,
              const int* _geometry, const bool* _smoothing,
              const int* _textures, const bool* _visible);
};

}
