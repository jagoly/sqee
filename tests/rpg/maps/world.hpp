#pragma once

#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/framebuffers.hpp>

#include "cell.hpp"
#include "obj/object.hpp"

namespace sqt {
namespace wld {

class World {
public:
    World();
    //~World();

    void load_base(const string& _filePath);

    void tick();
    void calc(double _accum);

    void activate_cell(const string& _cell);
    string get_move(glm::uvec2 _prev, glm::uvec2 _next);
    void set_player_pos(glm::uvec2 _pos);

    float get_maxZ4(const string& _layer, int _xPos, int _yPos);

    glm::vec3 minPos, maxPos;
    glm::uvec2 playerPos = {0, 0};

    list<Object*> objectList;
    list<Model*> modelList;
    list<Liquid*> liquidList;
    list<Data*> dataList;
    vector<Light*> lightVec;

    glm::vec3 ambiColour;
    int skylEnable;
    uint skylTexSize;
    glm::vec3 skylDir;
    glm::vec3 skylColour;
    glm::mat4 skylMat;
    uint spotCount;

    sq::UniformBuffer ubo;
    sq::Framebuffer slFb;
    array<sq::Framebuffer, 8> spFbArr;

private:
    map<string, Cell> cellMap;
    map<string, vector<pair<string, string>>> hlMap;
};

}
}
