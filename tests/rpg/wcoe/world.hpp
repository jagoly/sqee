#pragma once
#include <sqee/forward.hpp>

#include "cell.hpp"
#include "obj/object.hpp"

namespace sqt {
class MainCamera;
namespace wcoe {

class World : NonCopyable {
public:
    void add_cell(const string& _name, vec3 _position);
    Cell& get_cell(const string& _name);

    void tick();
    void calc(double _accum);

    void enable_cell(const string& _cell);
    void disable_cell(const string& _cell);
    bool get_cell_enabled(const string& _cell);
    forward_list<weak_ptr<Object>> objectList;

    template<class T>
    forward_list<weak_ptr<T>> filtered();

    unordered_map<string, shared_ptr<Cell>> cellMap;

    MainCamera* camera = nullptr;
    sq::SettingMap* settings = nullptr;
};

template<class T>
forward_list<weak_ptr<T>> World::filtered() {
    forward_list<weak_ptr<T>> retList;
    for (const auto& obj : objectList)
        if (obj.lock()->type == ObjTraits<T>::type())
            retList.emplace_front(static_wptr_cast<T>(obj));
    return retList;
}

}}
