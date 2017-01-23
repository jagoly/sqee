#pragma once

#include <sqee/builtins.hpp>
#include <sqee/app/MessageBus.hpp>

#include "../systems/Animation.hpp"

#include "objects/Camera.hpp"
#include "objects/SkyBox.hpp"
#include "objects/Ambient.hpp"
#include "objects/SkyLight.hpp"

#include "../messages.hpp"

// Forward Declarations /////
//namespace reactphysics3d { class DynamicsWorld; }

namespace sqt {

struct SceneData
{
    const world::CameraObject* camera = nullptr;
    const world::SkyBoxObject* skybox = nullptr;
    const world::AmbientObject* ambient = nullptr;
    const world::SkyLightObject* skylight = nullptr;
};

class World final {
public:
    World(sq::MessageBus& _messageBus);

    ~World();

    void update_options();
    void tick(); void update();

    //sq::EntityManager& get_EntityManager();
    //rp3d::DynamicsWorld& get_PhysicsWorld();

    //const sq::EntityManager& get_EntityManager() const;
    //const rp3d::DynamicsWorld& get_PhysicsWorld() const;

//    TransformSystem& get_TransformSystem() { return transformSystem; }
//    SoundSystem& get_SoundSystem() { return soundSystem; }

    SceneData sceneData;

    unique_ptr<world::CameraObject> camera;
    unique_ptr<world::SkyBoxObject> skybox;
    unique_ptr<world::AmbientObject> ambient;
    unique_ptr<world::SkyLightObject> skylight;

    float tickPercent = 0.f;

private:
    sq::MessageBus& messageBus;

    sq::Receiver<msg::Enable_SkyBox> on_Enable_SkyBox;
    sq::Receiver<msg::Enable_Ambient> on_Enable_Ambient;
    sq::Receiver<msg::Enable_SkyLight> on_Enable_SkyLight;

    sq::Receiver<msg::Disable_SkyBox> on_Disable_SkyBox;
    sq::Receiver<msg::Disable_Ambient> on_Disable_Ambient;
    sq::Receiver<msg::Disable_SkyLight> on_Disable_SkyLight;

    sq::Receiver<msg::Debug_1> on_Debug_1;
    sq::Receiver<msg::Debug_2> on_Debug_2;
    sq::Receiver<msg::Debug_3> on_Debug_3;
    sq::Receiver<msg::Debug_4> on_Debug_4;

//    AnimationSystem animationSystem;
//    TransformSystem transformSystem;
//    SoundSystem soundSystem;

    struct Impl;
    friend struct Impl;
    unique_ptr<Impl> impl;
};

}
