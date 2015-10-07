#pragma once
#include <sqee/forward.hpp>
#include <unordered_map>

#include <sqee/physics/RP3D.hpp>

#include "Cell.hpp"
#include "Object.hpp"
#include "Animation.hpp"

namespace sqt { namespace wcoe {

class SkyBox : NonCopyable {
public:
    SkyBox(sq::Camera* _camera);
    void refresh(); void tick();
    void calc(double _accum);

    bool   PROP_enabled    = false;
    float  PROP_saturation = 1.f;
    float  PROP_brightness = 0.f;
    float  PROP_contrast   = 1.f;
    float  PROP_alpha      = 1.f;
    string PROP_texture    = "";

    AnimatorFloat ANIM_saturation {&PROP_saturation};
    AnimatorFloat ANIM_brightness {&PROP_brightness};
    AnimatorFloat ANIM_contrast   {&PROP_contrast};
    AnimatorFloat ANIM_alpha      {&PROP_alpha};
    void animate();

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
    SkyLight(const sq::Settings& _settings, sq::Camera* _camera);
    void refresh(); void tick();
    void calc(double _accum);

    bool  PROP_enabled   = false;
    fvec3 PROP_direction = {0.f, 0.f, -1.f};
    fvec3 PROP_colour    = {1.f, 1.f, 1.f};
    float PROP_density   = 0.25f;

    AnimatorFNorm ANIM_direction {&PROP_direction};
    AnimatorFVec3 ANIM_colour    {&PROP_colour};
    AnimatorFloat ANIM_density   {&PROP_density};
    void animate();

    const sq::Settings& settings;
    const sq::Camera* const camera;
    unique_ptr<sq::UniformBuffer> ubo;
    unique_ptr<sq::Texture2DArray> texDepthA;
    unique_ptr<sq::Texture2DArray> texDepthB;
    array<sq::OrthoFrus, 4> orthArrA;
    array<sq::OrthoFrus, 2> orthArrB;
    array<fmat4, 4> matArrA;
    array<fmat4, 2> matArrB;
    fvec4 splits;
};


class World final : NonCopyable {
public:
    World(const sq::Settings& _settings, sq::Camera* _camera);
    void refresh(); void tick();
    void calc(double _accum);
    void invalidate();
    ~World();

    Cell* add_cell(const string& _name);
    Cell* get_cell(const string& _name);

    template<class T>
    vector<T*> filtered() const;
    vector<Object*> objectList;
    void reload_list();

    SkyBox skybox; Ambient ambient; SkyLight skylight;
    std::unordered_map<string, unique_ptr<Cell>> cellMap;

    const sq::Settings& settings;
    const sq::Camera* const camera;

    unique_ptr<rp3d::DynamicsWorld> physWorld;

};

template<class T>
vector<T*> World::filtered() const {
    vector<T*> retVec;
    for (Object* obj : objectList)
        if (typeid(*obj) == typeid(T))
            retVec.push_back(static_cast<T*>(obj));\
    return retVec;
}

}}
