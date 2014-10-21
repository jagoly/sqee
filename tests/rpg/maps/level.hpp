#pragma once

#include <map>
#include <string>
#include <vector>

#include <sqee/gl/gl.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/misc/containers.hpp>

#include "obj/object.hpp"

namespace sqt {

class Level {
public:
    void set_holders(obj::MeshHolder* _meshH, obj::SkinHolder* _skinH, sq::TexHolder* _texH);

    glm::uvec3 size;

    std::map<std::string, std::unique_ptr<obj::Object>> objectMap;

    bool load_map(std::string dirPath);

    bool load_objects();
    bool load_physics();

    void tick(int _tickRate);
    void update_render(float _ft);

    float get_subtile_z(int _x, int _y, std::string _layer);
    std::string get_join(int _x, int _y, std::string _layer);

    obj::MeshHolder* meshH;
    obj::SkinHolder* skinH;
    sq::TexHolder* texH;

private:
    std::string mapPath;
    Json::Value root;
    std::map<std::string, std::pair<glm::ivec3, std::vector<std::vector<std::array<uchar, 16>>>>> heightMaps;
    std::map<std::pair<std::string, std::pair<int,int>>, std::pair<std::string, std::pair<int,int>>> joinMap;
};

}
