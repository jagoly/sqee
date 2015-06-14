#pragma once
#include <sqee/forward.hpp>

#include "cell.hpp"
#include "obj/object.hpp"
#include "obj/animation.hpp"

namespace sqt { namespace wcoe {

class SkyBox : NonCopyable {
public:
    SkyBox(sq::Camera* _camera);
    void refresh(); void tick();
    void calc(double _accum);

    bool   PROP_enabled = false;
    fvec4  PROP_colour  = {1.f, 1.f, 1.f, 1.f};
    string PROP_texPath = "";

    // animators todo

    const sq::Camera* const camera;
    unique_ptr<sq::Uniformbuffer> ubo;
    sq::TextureCube* tex = nullptr;
};

class Ambient : NonCopyable {
public:
    Ambient(sq::Camera* _camera);
    void refresh(); void tick();
    void calc(double _accum);

    bool  PROP_enabled = false;
    fvec3 PROP_colour  = {0.5f, 0.5f, 0.5f};

    AnimatorFVec3 ANIM_colour {&PROP_colour};
    void animate();

    const sq::Camera* const camera;
    unique_ptr<sq::Uniformbuffer> ubo;
};

class SkyLight : NonCopyable {
public:
    SkyLight(sq::Camera* _camera);
    void refresh(); void tick();
    void calc(double _accum);

    bool  PROP_enabled   = false;
    fvec3 PROP_direction = {0.f, 0.f, -1.f};
    fvec3 PROP_colour    = {1.f, 1.f, 1.f};

    AnimatorFNorm ANIM_direction {&PROP_direction};
    AnimatorFVec3 ANIM_colour    {&PROP_colour};
    void animate();

    const sq::Camera* const camera;
    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::Texture2DArray> texA;
    unique_ptr<sq::Texture2DArray> texB;
    array<unique_ptr<sq::Framebuffer>, 4> fboArrA;
    array<unique_ptr<sq::Framebuffer>, 2> fboArrB;
    array<fmat4, 4> matArrA;
    array<fmat4, 2> matArrB;
    fvec4 splits;
};


class World : NonCopyable {
public:
    World(sq::Camera* _camera, sq::SettingsMaps* _settings);

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

    const sq::Camera* const camera;
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
