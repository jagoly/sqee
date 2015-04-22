#pragma once
#include <sqee/forward.hpp>

#include <forward_list>
#include <unordered_map>

#include "cell.hpp"
#include "obj/object.hpp"

namespace sqt {
class MainCamera;
namespace wcoe {

class World : NonCopyable {
public:
    void add_cell(const string& _name, vec3 _position);
    void add_cell(const string& _name, vec3 _position, const string& _path);

    void tick();
    void calc(double _accum);

    void enable_cell(const string& _cell);
    void disable_cell(const string& _cell);
    bool get_cell_enabled(const string& _cell);
    std::forward_list<weak_ptr<Object>> objectList;

    template<class T>
    std::forward_list<weak_ptr<T>> filtered();

    std::unordered_map<string, shared_ptr<Cell>> cellMap;

    MainCamera* camera = nullptr;
    sq::SettingMap* settings = nullptr;
};

template<class T>
std::forward_list<weak_ptr<T>> World::filtered() {
    std::forward_list<weak_ptr<T>> retList;
    for (const auto& obj : objectList)
        if (obj.lock()->type == ObjTraits<T>::type())
            retList.emplace_front(static_wptr_cast<T>(obj));
    return retList;
}

}}
