#pragma once

#include <vector>
#include <string>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/resources/texholder.hpp>

#include "../objects/models.hpp"
#include "terrain.hpp"
#include "models.hpp"

namespace sqt {

class Level {
public:
    Terrain terrain;
    glm::uvec2 size;
    int modelTexCount;

    std::vector<Model> modelVec;
    std::vector<ModelInstance> modelInstVec;
    std::vector<float> tileZs;

    bool load_map(std::string dirPath);

    bool load_ground();
    bool load_models();
    bool load_physics();

    float get_tile_z(int _x, int _y, ushort _layer);
private:
    std::string mapPath;
    sq::TexHolderBasic texHolder2D;
};

}
