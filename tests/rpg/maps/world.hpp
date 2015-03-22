#pragma once
#include <sqee/forward.hpp>

#include <forward_list>
#include <map>
#include <vector>

#include <sqee/gl/uniformbuffers.hpp>

#include "cell.hpp"
#include "obj/object.hpp"
#include "obj/light.hpp"
#include "obj/liquid.hpp"
#include "obj/model.hpp"
#include "obj/reflector.hpp"

namespace sqt {

class World : NonCopyable {
public:
    World();

    void load_base(const string& _filePath);
    void tick();
    void calc(double _accum);

    void set_active_tile(ivec2 _tile);
    void set_active_cell(const string& _cell);
    const ivec2& get_active_tile();
    const string& get_active_cell();

    float get_maxZ4(const string& _layer, int _xPos, int _yPos);

    vec3 minPos, maxPos;

    std::forward_list<Object*> objectList;
    template<class T>
    std::forward_list<T*> filtered() {
        return filter_objs<T>(objectList.begin(), objectList.end());
    }

    sq::Uniformbuffer ubo;

    vec3 ambiColour;

    struct {
        bool enable;
        vec3 colour;
        vec3 direction;
    } SKYL;

private:
    std::map<string, Cell> cellMap;
    std::map<string, std::vector<std::pair<string, string>>> hlMap;
};

}
