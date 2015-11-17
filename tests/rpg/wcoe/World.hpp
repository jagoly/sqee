#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/ecs/Entity.hpp>

#include "Cell.hpp"
#include "Object.hpp"
#include "Animation.hpp"

namespace sq { class Camera; }
namespace reactphysics3d { class DynamicsWorld; }

namespace sqt { namespace wcoe {

class SkyBox; class Ambient; class SkyLight;

class World final : sq::NonCopyable {
public:
    World(sq::MessageBus& _messageBus, const sq::Settings& _settings, const sq::Camera* _camera);
    ~World();

    void refresh(), update();
    void calc(double _accum);
    void invalidate();


    void update_stuff();
    sq::Entity root;


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

    sq::MessageBus& messageBus;
    const sq::Settings& settings;
    const sq::Camera* const camera;

    unique_ptr<rp3d::DynamicsWorld> physWorld;
};

template<class T>
vector<T*> World::filtered() const {
    vector<T*> retVec;
    for (Object* obj : objectList)
        if (obj->type == typeid(T))
            retVec.push_back(static_cast<T*>(obj));
    return retVec;
}

}}
