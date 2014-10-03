#include "level.hpp"

#include <libsqee/misc/files.hpp>

#include <libsqee/redist/stb_image.hpp>

using namespace sqt;

bool Level::load_map(std::string _mapPath) {
    mapPath = _mapPath;

    // TODO: Checks to make sure all resources required in map.json are available

    root = sq::get_json_from_file("res/maps/" + mapPath + "/map.json");

    size.x = root["aProps"]["xSize"].asUInt();
    size.y = root["aProps"]["ySize"].asUInt();
    size.z = root["aProps"]["zSize"].asUInt();

    return false;
}

bool Level::load_objects() {
    for (Json::Value& layer : root["dObjects"]) {
        objectMapVec.emplace_back();
        for (std::string& key : layer.getMemberNames()) {
            objectMapVec.back().insert({key, obj::create(layer[key], &meshH, &skinH, &texH, mapPath, key)});
        }
    }
    return false;
}

bool Level::load_physics() {
    for (std::string& key : root["bPhysics"].getMemberNames()) {
        int w, h;
        std::string fPath = "res/maps/" + mapPath + "/data/" + root["bPhysics"][key][0].asString()+".png";
        uchar* data = stbi_load(fPath.c_str(), &w, &h, nullptr, 1);
        if (data == NULL) {
            std::cout << "ERROR: Error loading height data from \"" << fPath << "\"" << std::endl;
            return true;
        }

        heightMaps[key].second.resize(h/4);
        for (std::vector<std::array<unsigned char, 16>>& vec : heightMaps[key].second) {
            vec.resize(w/4);
        }

        for (int ry = 0; ry < h; ry++) {
            int y = h-1 - ry;
            for (int x = 0; x < w; x++) {
                heightMaps[key].first.x = root["bPhysics"][key][1].asInt();
                heightMaps[key].first.y = root["bPhysics"][key][2].asInt();
                heightMaps[key].first.z = root["bPhysics"][key][3].asInt();
                heightMaps[key].second[y/4][x/4][y%4*4 + x%4] = data[ry*w + x];
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

void Level::tick(int _tickRate) {
    for (obj::MeshHolder::Pair& m : meshH) {
         m.second->tick();
    }
    for (std::map<std::string, obj::Ptr>& l : objectMapVec) {
        for (std::pair<const std::string, obj::Ptr>& o : l) {
            o.second->tick(_tickRate);
        }
    }
}

void Level::update_render(float _ft) {
    for (obj::MeshHolder::Pair& mesh : meshH) {
        mesh.second->accum += _ft;
    }
}

float Level::get_subtile_z(int _x, int _y, std::string _layer) {
    return (float(heightMaps[_layer].second
                  [_y/4 - heightMaps[_layer].first.y][_x/4 - heightMaps[_layer].first.x][_y%4*4 + _x%4])
            + heightMaps[_layer].first.z) / 16.f;
}

std::string Level::get_join(int _x, int _y, std::string _layer) {
    std::pair<std::string, std::pair<int,int>> val = {_layer, {_x - heightMaps[_layer].first.x, _y - heightMaps[_layer].first.y}};
    if (joinMap.count(val)) {
        return joinMap[val].first;
    }

    return _layer;
}
