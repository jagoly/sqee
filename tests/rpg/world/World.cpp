#include "../systems/Entity.hpp"
#include "../systems/Animation.hpp"
#include "../systems/Transform.hpp"
#include "../systems/Render.hpp"
#include "../systems/Culling.hpp"
#include "../systems/Sound.hpp"

//#include <rp3d/engine/DynamicsWorld.hpp>

#include "World.hpp"

using namespace sqt;
namespace maths = sq::maths;

struct World::Impl {
    //rp3d::DynamicsWorld physicsWorld {{0, 0, -1}};
};


World::World(sq::MessageBus& _messageBus)
    : messageBus(_messageBus),
      //entityManager(_messageBus),
      impl(new Impl()) {

    camera = std::make_unique<world::CameraObject>();

    //impl->physicsWorld.setNbIterationsVelocitySolver(18u);
    //impl->physicsWorld.setNbIterationsPositionSolver(12u);


    on_Enable_SkyBox.func = [this](auto) { skybox = std::make_unique<world::SkyBoxObject>(); };
    on_Enable_Ambient.func = [this](auto) { ambient = std::make_unique<world::AmbientObject>(); };
    on_Enable_SkyLight.func = [this](auto) { skylight = std::make_unique<world::SkyLightObject>(); };

    on_Disable_SkyBox.func = [this](auto) { skybox.reset(nullptr); };
    on_Disable_Ambient.func = [this](auto) { ambient.reset(nullptr); };
    on_Disable_SkyLight.func = [this](auto) { skylight.reset(nullptr); };

    messageBus.subscribe_back(on_Enable_SkyBox);
    messageBus.subscribe_back(on_Enable_Ambient);
    messageBus.subscribe_back(on_Enable_SkyLight);

    messageBus.subscribe_back(on_Disable_SkyBox);
    messageBus.subscribe_back(on_Disable_Ambient);
    messageBus.subscribe_back(on_Disable_SkyLight);

    // temp debug stuff

    //on_Debug_1.func = [=](auto) { sys::static_SoundSystem().play("effects/Whack", 1u); };
    //on_Debug_2.func = [=](auto) { sys::static_SoundSystem().play("effects/Whack", 1u, Vec3F(0,0,5)); };
    //messageBus.subscribe_back(on_Debug_1);
    //messageBus.subscribe_back(on_Debug_2);
}

World::~World() = default;


//rp3d::DynamicsWorld& World::get_PhysicsWorld() { return impl->physicsWorld; }

//const rp3d::DynamicsWorld& World::get_PhysicsWorld() const { return impl->physicsWorld; }


void World::update_options() {
    //entityManager.configure_complete();
}

void World::tick()
{
    //impl->rootEntity.propogate_forward<AnimatorComponent>    ( tick_wrapper );
    //impl->rootEntity.propogate_forward<DynamicBodyComponent> ( tick_wrapper );

    //impl->physicsWorld.update(1.f / 30.f);

    auto& stuff = sys::static_WorldStuff();

    sys::system_tick_animations(stuff);
}

void World::update()
{
    auto& stuff = sys::static_WorldStuff();

    sys::system_refresh_nesting(stuff);
    sys::system_refresh_transforms(stuff);
    sys::system_refresh_combo_sets(stuff);
    sys::system_refresh_culling(stuff);

    //sys::static_SoundSystem().system_refresh();

    sceneData.camera = camera.get();
    sceneData.skybox = skybox.get();
    sceneData.ambient = ambient.get();
    sceneData.skylight = skylight.get();
}
