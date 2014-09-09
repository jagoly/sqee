#include "level.hpp"

#include <libsqee/misc/files.hpp>

using namespace sqt;


bool Level::load_map(std::string _mapPath) {
    mapPath = _mapPath;

    // Put some kind of checks here
    return false;
}

bool Level::load_ground() {
    Json::Value rootProps = sq::get_json_from_file("res/maps/" + mapPath + "/props.json");
    Json::Value rootTerrain = sq::get_json_from_file("res/maps/" + mapPath+"/terrain.json");

    size.x = rootProps["width"].asInt();
    size.y = rootProps["height"].asInt();

    int terrainGeometry[(size.x +1) * (size.y +1)];
    for (uint i = 0; i < (size.x +1) * (size.y +1); i++) {
        terrainGeometry[i] = rootTerrain["geometry"][i].asInt();
    }

    int terrainTextures[size.x * size.y];
    bool terrainSmoothing[size.x * size.y];
    bool terrainVisible[size.x * size.y];
    for (uint i = 0; i < size.x * size.y; i++) {
        terrainTextures[i] = rootTerrain["textures"][i].asInt();
        terrainSmoothing[i] = rootTerrain["smoothing"][i].asBool();
        terrainVisible[i] = rootTerrain["visible"][i].asBool();
    }

    int terrainTexCount = rootProps["terrainTextures"].size();
    std::vector<std::string> terrainTexPaths;
    terrainTexPaths.resize(terrainTexCount);
    for (int i = 0; i < terrainTexCount; i++) {
        terrainTexPaths[i] = rootProps["terrainTextures"][i].asString();
    }

    terrain.load(size, terrainTexCount, terrainTexPaths, mapPath, terrainGeometry,
                 terrainSmoothing, terrainTextures, terrainVisible);

    return false;
}

bool Level::load_models() {
    Json::Value root = sq::get_json_from_file("res/maps/" + mapPath + "/models.json");

    modelVec.resize(root["instances"].size());
    int i = 0;
    for (Json::Value& val : root["instances"]) {
        std::cout << val["mesh"].asInt() << "  " << val["skin"].asInt() << std::endl;
        modelVec[i++].load_from_dir(root["meshes"][val["mesh"].asInt()].asString(),
                                    root["skins"][val["skin"].asInt()].asString(),
                                    &texHolder2D);
    }

    i = 0;
    for (Json::Value& val : root["instances"]) {
        int ambi = modelVec[i].skin.ambi;
        int i2 = 0;
        for (Json::Value& loc : val["locations"]) {
            int arg = i2++;
            if (ambi == 1)      arg = -1;
            else if (ambi == 2) arg = -2;
            modelInstVec.emplace_back();
            modelInstVec.back().create({loc[0].asFloat(), loc[1].asFloat(), loc[2].asFloat()},
                                       i, arg, mapPath);
        }
        i++;
    }

    return false;
}

bool Level::load_physics() {
    Json::Value root = sq::get_json_from_file("res/maps/" + mapPath + "/physics.json");

    for (Json::Value& val : root["terrainZ"]) {
        tileZs.push_back(val.asFloat());
    }

    return false;
}

float Level::get_tile_z(int _x, int _y, ushort _layer) {
    if (_layer == 0) {
        return tileZs[_y * size.x + _x];
    }
    return 1.f; // todo
}
