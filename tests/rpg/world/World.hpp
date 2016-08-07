#pragma once

#include <sqee/builtins.hpp>
#include <sqee/ecs/Entity.hpp>
#include <sqee/ecs/Manager.hpp>

#include "static/StaticCell.hpp"

#include "../systems/Animation.hpp"
#include "../systems/Transform.hpp"

#include "objects/Camera.hpp"
#include "objects/SkyBox.hpp"
#include "objects/Ambient.hpp"
#include "objects/SkyLight.hpp"

#include "../messages.hpp"

// Forward Declarations /////
namespace reactphysics3d { class DynamicsWorld; }

namespace sqt {

struct SceneData {

    const world::StaticCellMap* staticCellMap = nullptr;

    const world::CameraObject* camera = nullptr;
    const world::SkyBoxObject* skybox = nullptr;
    const world::AmbientObject* ambient = nullptr;
    const world::SkyLightObject* skylight = nullptr;

    //vector<const sq::Entity*> entities;
};

class World final {
public:
    World(sq::MessageBus& _messageBus);

    ~World();

    void update_options();
    void tick(); void update();

    world::StaticCell* add_StaticCell(const string& _name);
    const world::StaticCell& get_StaticCell(const string& _name) const;
    //const world::StaticCellMap& get_StaticCellMap() const { return staticCellMap; }

    sq::EntityManager& get_EntityManager();
    rp3d::DynamicsWorld& get_PhysicsWorld();

    const sq::EntityManager& get_EntityManager() const;
    const rp3d::DynamicsWorld& get_PhysicsWorld() const;

    SceneData sceneData;

    unique_ptr<world::CameraObject> camera;
    unique_ptr<world::SkyBoxObject> skybox;
    unique_ptr<world::AmbientObject> ambient;
    unique_ptr<world::SkyLightObject> skylight;

    float tickPercent = 0.f;

private:
    sq::MessageBus& messageBus;

    world::StaticCellMap staticCellMap;

    sq::Receiver<msg::Configure_Entity> on_Configure_Entity;

    sq::Receiver<msg::Enable_SkyBox> on_Enable_SkyBox;
    sq::Receiver<msg::Enable_Ambient> on_Enable_Ambient;
    sq::Receiver<msg::Enable_SkyLight> on_Enable_SkyLight;

    sq::Receiver<msg::Disable_SkyBox> on_Disable_SkyBox;
    sq::Receiver<msg::Disable_Ambient> on_Disable_Ambient;
    sq::Receiver<msg::Disable_SkyLight> on_Disable_SkyLight;

    sq::EntityManager entityManager;

    AnimationSystem animationSystem;
    TransformSystem transformSystem;

    struct Impl;
    friend struct Impl;
    unique_ptr<Impl> impl;
};

}
