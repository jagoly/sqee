#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>

#include <sqee/gl/gl.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/misc/containers.hpp>

#include "obj/object.hpp"

namespace sqt {

class Level {
public:
    // change to unique when qt creator fixed
    typedef std::shared_ptr<Level> Ptr;
    typedef std::pair<const std::string, std::unique_ptr<obj::Object>> SOPair;

    Level(obj::MeshHolder& _meshH, obj::SkinHolder& _skinH, sq::TexHolder& _texH)
        : meshH(_meshH), skinH(_skinH), texH(_texH) {}

    glm::uvec3 size;

    std::map<std::string, std::unique_ptr<obj::Object>> objectMap;

    bool load_map(const std::string& _mapPath);
    bool load_objects();
    bool load_physics();

    void tick();
    void calc(double _accum);

    float get_max4_z(int _x, int _y, const std::string& _layer);
    std::string get_join(int _x, int _y, const std::string& _layer);

    obj::MeshHolder& meshH;
    obj::SkinHolder& skinH;
    sq::TexHolder& texH;

private:
    std::string mapPath;
    Json::Value root;

    typedef std::vector<std::vector<std::array<uchar, 16>>> HeightLayer;
    std::map<std::string, std::pair<glm::ivec3, HeightLayer>> hlMap;

    std::map<std::pair<std::string, std::pair<int,int>>, std::pair<std::string, std::pair<int,int>>> joinMap;
};

}
