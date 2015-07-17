#pragma once
#include <sqee/forward.hpp>

#include <sqee/physics/RP3D.hpp>

#include "Cell.hpp"
#include "obj/Object.hpp"
#include "obj/Animation.hpp"

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
    unique_ptr<sq::UniformBuffer> ubo;
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
    unique_ptr<sq::UniformBuffer> ubo;
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
    unique_ptr<sq::UniformBuffer> ubo;
    unique_ptr<sq::TextureArray> texA;
    unique_ptr<sq::TextureArray> texB;
    array<unique_ptr<sq::FrameBuffer>, 4> fboArrA;
    array<unique_ptr<sq::FrameBuffer>, 2> fboArrB;
    array<sq::OrthoFrus, 4> orthArrA;
    array<sq::OrthoFrus, 2> orthArrB;
    array<fmat4, 4> matArrA;
    array<fmat4, 2> matArrB;
    fvec4 splits;
};


class World : NonCopyable {
public:
    World(sq::Camera* _camera, sq::SettingsMaps* _settings);
    void refresh(); void tick();
    void calc(double _accum);
    ~World();

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

    unique_ptr<rp3d::DynamicsWorld> physWorld;
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
