#pragma once

#include <vector>
#include <string>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/mesh.hpp>
#include <libsqee/textureholder.hpp>

namespace sqt {

struct Ground {
    int width, height;
    int texCount, pCount;

    GLuint vao, texArray, nMapArray;
};

struct ModelInstance {
    ModelInstance(glm::vec3 _pos, glm::vec3 _scale, int _index);
    glm::vec3 pos, scale;
    int index;
    glm::mat4 modelMatrix;
};

class LevelMap {
public:
    Ground ground;
    std::vector<sq::Mesh> modelVec;
    std::vector<ModelInstance> mapModelVec;

    bool load_map(std::string dirPath);

    bool load_ground();
    bool load_models();
private:
    std::string mapDirPath;
};

}
