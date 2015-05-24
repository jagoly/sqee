#pragma once
#include <sqee/forward.hpp>

#include "cell.hpp"
#include "obj/object.hpp"

namespace sqt {
class MainCamera;
namespace wcoe {

class SkyBox : NonCopyable {
public:
    SkyBox(MainCamera* _camera);
    void refresh(); void tick();
    void calc(double _accum);

    bool DAT_enabled = false;
    fvec4 DAT_colour = {1.f, 1.f, 1.f, 1.f};
    string DAT_texPath;

    unique_ptr<sq::Uniformbuffer> ubo;
    const MainCamera* const camera;

    sq::TextureCube* tex = nullptr;
};

class Ambient : NonCopyable {
public:
    Ambient(MainCamera* _camera);
    void refresh(); void tick();
    void calc(double _accum);

    bool DAT_enabled = false;
    fvec3 DAT_colour = {0.5f, 0.5f, 0.5f};

    unique_ptr<sq::Uniformbuffer> ubo;
    const MainCamera* const camera;
};

class SkyLight : NonCopyable {
public:
    SkyLight(MainCamera* _camera);
    void refresh(); void tick();
    void calc(double _accum);

    bool DAT_enabled = false;
    fvec3 DAT_colour = {1.f, 1.f, 1.f};
    fvec3 DAT_normal = {0.f, 0.f, -1.f};

    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::Texture2DArray> texA;
    unique_ptr<sq::Texture2DArray> texB;
    array<unique_ptr<sq::Framebuffer>, 4> fboArrA;
    array<unique_ptr<sq::Framebuffer>, 2> fboArrB;
    const MainCamera* const camera;

    array<fmat4, 4> matArrA; array<float, 4> splitArrA;
    array<fmat4, 2> matArrB; array<float, 2> splitArrB;
};


class World : NonCopyable {
public:
    World(MainCamera* _camera, sq::SettingsMaps* _settings);

    void refresh(); void tick();
    void calc(double _accum);

    Cell* add_cell(const string& _name);
    Cell* get_cell(const string& _name);

    template<class T>
    forward_list<weak_ptr<T>> filtered();
    forward_list<weak_ptr<Object>> objectList;
    void reload_list();

    SkyBox skybox; Ambient ambient; SkyLight skylight;
    unordered_map<string, shared_ptr<Cell>> cellMap;

    const MainCamera* const camera;
    sq::SettingsMaps* const settings;
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
