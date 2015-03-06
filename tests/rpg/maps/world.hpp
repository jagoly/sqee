#pragma once
#include <sqee/forward.hpp>

#include <forward_list>
#include <map>
#include <vector>


#include <sqee/gl/uniformbuffers.hpp>

#include "cell.hpp"
#include "obj/object.hpp"
#include "obj/model.hpp"
#include "obj/liquid.hpp"
#include "obj/reflector.hpp"
#include "obj/light.hpp"

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

    std::forward_list<Object*> objectList;
    template<class T>
    std::forward_list<T*> filtered(std::function<bool(const T&)>&& _func = AlwaysTrue<T>()) {
        return filter_objs(objectList.begin(), objectList.end(), _func);
    }

    glm::vec3 ambiColour;
    bool skylEnable;
    glm::vec3 skylDir;
    glm::vec3 skylColour;
    std::vector<glm::mat4> skylMatVec;
    uint spotCount;

    sq::UniformBuffer ubo;

    bool updateScene = false;

private:
    std::map<std::string, Cell> cellMap;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> hlMap;

    sq::Application& app;
};

}
