#pragma once

#include <map>
#include <string>
#include <vector>

#include <boost/bimap.hpp>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/resources/resholder.hpp>

#include "obj/object.hpp"

namespace sqt {

class Level {
public:
    glm::uvec3 size;

    std::vector<std::map<std::string, obj::Ptr>> objectMapVec;

    bool load_map(std::string dirPath);

    bool load_objects();
    bool load_physics();

    void tick(int _tickRate);
    void update_render(float _ft);

    float get_subtile_z(int _x, int _y, std::string _layer);
    std::string get_join(int _x, int _y, std::string _layer);

    obj::MeshHolder meshH;
    obj::SkinHolder skinH;
    sq::TexHolder texH;
private:
    std::string mapPath;
    Json::Value root;
    std::map<std::string, std::pair<glm::ivec3, std::vector<std::vector<std::array<unsigned char, 16>>>>> heightMaps;
    std::map<std::pair<std::string, std::pair<int,int>>, std::pair<std::string, std::pair<int,int>>> joinMap;
};

}
