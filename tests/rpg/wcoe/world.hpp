#pragma once
#include <sqee/forward.hpp>

#include "cell.hpp"
#include "obj/object.hpp"

namespace sqt {
class MainCamera;
namespace wcoe {

class SkyBox : NonCopyable {
public:
    SkyBox(); void tick();
    void calc(double _accum);
    bool enabled = false;

    unique_ptr<sq::Uniformbuffer> ubo;
    const MainCamera* camera = nullptr;

    vec3 colour = {1.f, 1.f, 1.f};
    sq::TextureCube* tex = nullptr;

    void set_colour(const vec3& _colour);
    void set_texture(const string& _path);
};

class Ambient : NonCopyable {
public:
    Ambient(); void tick();
    void calc(double _accum);
    bool enabled = false;

    unique_ptr<sq::Uniformbuffer> ubo;
    const MainCamera* camera = nullptr;

    vec3 colour = {0.5f, 0.5f, 0.5f};

    void set_colour(const vec3& _colour);
};

class SkyLight : NonCopyable {
public:
    SkyLight(); void tick();
    void calc(double _accum);
    bool enabled = false;

    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::Texture2DArray> tex;
    array<unique_ptr<sq::Framebuffer>, 4> fboArr;
    const MainCamera* camera = nullptr;

    vec3 colour = {1.f, 1.f, 1.f};
    vec3 direction = {0.f, 0.f, -1.f};

    void set_colour(const vec3& _colour);
    void set_direction(const vec3& _direction);

    array<mat4, 4> matArr;
    array<float, 4> splitArr;
    array<sq::Frustum, 4> frusArr;
};


class World : NonCopyable {
public:
    void add_cell(const string& _name, vec3 _position);
    Cell& get_cell(const string& _name);

    void tick();
    void calc(double _accum);

    void enable_cell(const string& _cell);
    void disable_cell(const string& _cell);
    forward_list<weak_ptr<Object>> objectList;

    template<class T>
    forward_list<weak_ptr<T>> filtered();

    SkyBox skybox; Ambient ambient; SkyLight skylight;
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
