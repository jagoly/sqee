#include "level.hpp"

#include <libsqee/misc/files.hpp>

using namespace sqt;


bool Level::load_map(std::string _dirPath) {
    mapDirPath = _dirPath;

    // Put some kind of checks here
    return false;
}

bool Level::load_ground() {
    Json::Value rootProps = sq::get_json_from_file(mapDirPath+"/props.json");
    Json::Value rootTerrain = sq::get_json_from_file(mapDirPath+"/terrain.json");

    size.x = rootProps["width"].asInt();
    size.y = rootProps["height"].asInt();

    int terrainGeometry[(size.x +1) * (size.y +1)];
    for (int i = 0; i < (size.x +1) * (size.y +1); i++) {
        terrainGeometry[i] = rootTerrain["geometry"][i].asInt();
    }

    int terrainTextures[size.x * size.y];
    bool terrainSmoothing[size.x * size.y];
    bool terrainVisible[size.x * size.y];
    for (int i = 0; i < size.x * size.y; i++) {
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

    terrain.load(size, terrainTexCount, terrainTexPaths,terrainGeometry,
                 terrainSmoothing, terrainTextures, terrainVisible);

    return false;
}

bool Level::load_models() {
    Json::Value root = sq::get_json_from_file(mapDirPath+"/models.json");

    modelVec.resize(root["models"].size());
    for (int i = 0; i < modelVec.size(); i++) {
        modelVec[i].load_from_dir("res/models/" + root["models"][i][0].asString(),
                                      root["models"][i][1].asString());
    }

    for (Json::Value& val : root["instances"]) {
        for (Json::Value& loc : val["locations"]) {
            modelInstVec.push_back(ModelInstance(
                {loc[0].asFloat(), loc[1].asFloat(), loc[2].asFloat()},
                val["index"].asInt()));
        }
    }

    return false;
}
