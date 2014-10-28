#include <iostream>

#include <sqee/redist/stb_image.hpp>
#include <sqee/misc/files.hpp>

#include "level.hpp"

using namespace sqt;

bool Level::load_map(const std::string& _mapPath) {
    mapPath = _mapPath;

    // TODO: Checks to make sure all resources required in map.json are available

    root = sq::get_json_from_file("res/maps/" + mapPath + "/map.json");

    size.x = root["aProps"]["xSize"].asUInt();
    size.y = root["aProps"]["ySize"].asUInt();
    size.z = root["aProps"]["zSize"].asUInt();

    return false;
}

bool Level::load_objects() {
    for (const std::string& key : root["dObjects"].getMemberNames()) {
        objectMap[key] = std::unique_ptr<obj::Object>(
                    obj::create(root["dObjects"][key], meshH, skinH, texH, mapPath, key));
    }
    return false;
}

bool Level::load_physics() {
    for (std::string& key : root["bPhysics"].getMemberNames()) {
        int w, h;
        std::string fPath = "res/maps/" + mapPath + "/data/" + root["bPhysics"][key][0].asString()+".png";
        uchar* data = stbi_load(fPath.c_str(), &w, &h, nullptr, 1);
        if (!data) {
            std::cout << "ERROR: Error loading height data from \"" << fPath << "\"" << std::endl;
            free(data); return true;
        }

        hlMap[key].second.resize(h/4);
        for (std::vector<std::array<unsigned char, 16>>& vec : hlMap[key].second) {
            vec.resize(w/4);
        }

        for (int ry = 0; ry < h; ry++) {
            int y = h-1 - ry;
            for (int x = 0; x < w; x++) {
                hlMap[key].first.x = root["bPhysics"][key][1].asInt();
                hlMap[key].first.y = root["bPhysics"][key][2].asInt();
                hlMap[key].first.z = root["bPhysics"][key][3].asInt();
                hlMap[key].second[y/4][x/4][y%4*4 + x%4] = data[ry*w + x];
            }
        }
        free(data);
    }

    for (Json::Value& val : root["cJoins"]) {
        std::pair<std::string, std::pair<int,int>> v1 = {val[0][0].asString(), {val[0][1].asInt(), val[0][2].asInt()}};
        std::pair<std::string, std::pair<int,int>> v2 = {val[1][0].asString(), {val[1][1].asInt(), val[1][2].asInt()}};
        joinMap[v1] = v2;
    }

    return false;
}

void Level::tick() {
    for (std::pair<const std::string, std::unique_ptr<obj::Object>>& o : objectMap) {
        o.second->tick();
    }
}

void Level::calc(double _accum) {

}

float Level::get_max4_z(int _x, int _y, const std::string& _layer) {
    std::pair<glm::ivec3, HeightLayer>& hlPair = hlMap[_layer];

    int xT = _x/4 - hlPair.first.x,
        yT = _y/4 - hlPair.first.y;
    char xO = _x%4,
         yO = _y%4;

    int xT_0 = xT,
        xT_1 = xT;
    char xO_0 = xO-1,
         xO_1 = xO;
    int yT_0 = yT,
        yT_1 = yT;
    char yO_0 = yO-1,
         yO_1 = yO;

    if (xO_0 == -1) { xO_0 = 3; xT_0--; }
    else
    if (xO_1 == 4)  { xO_1 = 0; xT_1++; }

    if (yO_0 == -1) { yO_0 = 3; yT_0--; }
    else
    if (yO_1 == 4)  { yO_1 = 0; yT_1++; }

    uchar val = hlPair.second[yT_0][xT_0][yO_0*4 + xO_0];
    val = std::max(val, hlPair.second[yT_0][xT_1][yO_0*4 + xO_1]);
    val = std::max(val, hlPair.second[yT_1][xT_0][yO_1*4 + xO_0]);
    val = std::max(val, hlPair.second[yT_1][xT_1][yO_1*4 + xO_1]);

    return float(hlPair.first.z + val) / 16.f;
}

std::string Level::get_join(int _x, int _y, const std::string& _layer) {
    std::pair<std::string, std::pair<int,int>> val = {_layer, {_x - hlMap[_layer].first.x, _y - hlMap[_layer].first.y}};
    if (joinMap.count(val)) {
        return joinMap[val].first;
    }

    return _layer;
}
