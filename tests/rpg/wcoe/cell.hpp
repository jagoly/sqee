#pragma once
#include <sqee/forward.hpp>

namespace sqt { namespace wcoe {

class Object;
class World;

class Cell : NonCopyable {
public:
    Cell(const string& _name, World* _world);
    void refresh(); void tick();
    void calc(double _accum);

    const string name;
    World* const world;

    bool DAT_enabled = false;
    fvec3 DAT_position = {0, 0, 0};

    template<class T>
    T* add_object(const string& _name);
    template<class T = Object>
    T* get_object(const string& _name);

    unordered_map<string, shared_ptr<Object>> objectMap;
    void load_from_file(const string& _path);
};

template<class T>
T* Cell::add_object(const string& _name) {
    Object* ptr = new T(_name, this);
    objectMap.emplace(_name, shared_ptr<Object>(ptr));
    return static_cast<T*>(objectMap.at(_name).get());
}

template<class T>
T* Cell::get_object(const string& _name) {
    return static_cast<T*>(objectMap.at(_name).get());
}

}}
