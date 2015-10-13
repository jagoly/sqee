#pragma once
#include <sqee/forward.hpp>
#include <unordered_map>

#include "Cell.hpp"
#include "Object.hpp"
#include "Animation.hpp"

namespace reactphysics3d { class DynamicsWorld; }

namespace sqt { namespace wcoe {

class SkyBox; class Ambient; class SkyLight;

class World final : NonCopyable {
public:
    World(const sq::Settings& _settings, const sq::Camera* _camera);
    ~World();

    void refresh(), update();
    void calc(double _accum);
    void invalidate();

    Cell* add_cell(const string& _name);
    Cell* get_cell(const string& _name);

    template<class T>
    vector<T*> filtered() const;
    vector<Object*> objectList;
    void reload_list();

    unique_ptr<SkyBox> skybox;
    unique_ptr<Ambient> ambient;
    unique_ptr<SkyLight> skylight;
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
            retVec.push_back(static_cast<T*>(obj));
    return retVec;
}

}}
