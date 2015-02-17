#pragma once
#include <sqee/forward.hpp>

#include <list>
#include <map>
#include <vector>

#include <sqee/gl/uniformbuffers.hpp>

#include "cell.hpp"
#include "obj/object.hpp"

namespace sqt {

class World : NonCopyable {
public:
    World(sq::Application& _app);

    void load_base(const std::string& _filePath);

    void tick();
    void calc(double _accum);

    void set_active_tile(glm::ivec2 _tile);
    void set_active_cell(const std::string& _cell);
    const glm::ivec2& get_active_tile();
    const std::string& get_active_cell();

    float get_maxZ4(const std::string& _layer, int _xPos, int _yPos);

    glm::vec3 minPos, maxPos;

    std::list<Object*> objectList;
    std::list<Model*> modelList;
    std::list<Liquid*> liquidList;
    std::list<Reflector*> reflectorList;
    std::vector<Light*> lightVec;

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
    std::map<std::string, Cell> cellMap;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> hlMap;

    sq::Application& app;
};

}
