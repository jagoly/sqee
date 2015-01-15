#pragma once

#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/framebuffers.hpp>

#include "cell.hpp"
#include "obj/object.hpp"

namespace sqt {

class World {
public:
    World();
    //~World();

    void load_base(const string& _filePath);

    void tick();
    void calc(double _accum);

    void set_active_tile(glm::ivec2 _tile);
    void set_active_cell(const string& _cell);
    const glm::ivec2& get_active_tile();
    const string& get_active_cell();

    float get_maxZ4(const string& _layer, int _xPos, int _yPos);

    glm::vec3 minPos, maxPos;

    list<Object*> objectList;
    list<Model*> modelList;
    list<Liquid*> liquidList;
    list<Reflector*> reflectorList;
    vector<Light*> lightVec;

    glm::vec3 ambiColour;
    bool skylEnable;
    uint skylTexSize;
    glm::vec3 skylDir;
    glm::vec3 skylColour;
    glm::mat4 skylMat;
    uint spotCount;

    sq::UniformBuffer ubo;

    bool updateScene = false;

private:
    map<string, Cell> cellMap;
    map<string, vector<pair<string, string>>> hlMap;


};

}
