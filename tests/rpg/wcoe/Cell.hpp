#pragma once
#include <sqee/forward.hpp>
#include <unordered_map>

namespace sqt { namespace wcoe {

class Object;
class World;

class Cell final : NonCopyable {
public:
    Cell(const string& _name, World& _world);

    void refresh(), update();
    void calc(double _accum);
    void invalidate();

    const string name;
    World& world;

    bool PROP_enabled = false;
    fvec3 PROP_position = {0, 0, 0};

    template<class T>
    T* add_object(const string& _name);
    template<class T = Object>
    T* get_object(const string& _name);

    std::unordered_map<string, unique_ptr<Object>> objectMap;
    void load_from_file(const string& _path);
};

template<class T>
T* Cell::add_object(const string& _name) {
    Object* ptr = new T(_name, this);
    objectMap.emplace(_name, unique_ptr<Object>(ptr));
    return static_cast<T*>(objectMap.at(_name).get());
}

template<class T>
T* Cell::get_object(const string& _name) {
    return static_cast<T*>(objectMap.at(_name).get());
}

}}
