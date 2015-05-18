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

    vec4 colour = {1.f, 1.f, 1.f, 1.f};
    sq::TextureCube* tex = nullptr;

    void set_colour(const vec4& _colour);
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
    unique_ptr<sq::Texture2DArray> texA;
    unique_ptr<sq::Texture2DArray> texB;
    array<unique_ptr<sq::Framebuffer>, 4> fboArrA;
    array<unique_ptr<sq::Framebuffer>, 2> fboArrB;
    const MainCamera* camera = nullptr;

    vec3 colour = {1.f, 1.f, 1.f};
    vec3 direction = {0.f, 0.f, -1.f};

    void set_colour(const vec3& _colour);
    void set_direction(const vec3& _direction);

    array<mat4, 4> matArrA;
    array<mat4, 2> matArrB;
    array<float, 4> splitArrA;
    array<float, 2> splitArrB;
    array<sq::Frustum, 4> frusArrA;
    array<sq::Sphere, 2> sphrArrB;
};


class World : NonCopyable {
public:
    Cell* add_cell(const string& _name, vec3 _position);
    Cell* get_cell(const string& _name);

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
