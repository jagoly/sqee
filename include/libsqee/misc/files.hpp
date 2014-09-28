#pragma once

#include <glm/glm.hpp>

#include <tuple>
#include <jsoncpp/json/json.h>

namespace sq {

typedef std::pair<std::string, int> SIPair;

struct Vertex {
    Vertex(float _x, float _y, float _z, float _nx, float _ny, float _nz, float _s, float _t);
    float x, y, z, nx, ny, nz, s, t, tx, ty, tz;
};

Json::Value get_json_from_file(std::string _path);

std::string get_string_from_file(std::string _path);

bool load_ply_from_file(std::string _path, std::vector<Vertex>& _vertVec, std::vector<glm::uvec3>& _faceVec);

}
