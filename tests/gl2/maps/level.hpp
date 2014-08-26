#pragma once

#include <vector>
#include <string>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/models/mesh.hpp>

#include "terrain.hpp"
#include "models.hpp"

namespace sqt {

class Level {
public:
    Terrain terrain;
    glm::uvec2 size;
    int modelTexCount;

    std::vector<sq::Mesh> modelVec;
    std::vector<ModelInstance> modelInstVec;

    bool load_map(std::string dirPath);

    bool load_ground();
    bool load_models();
private:
    std::string mapPath;
};

}
