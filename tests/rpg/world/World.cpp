#include <rp3d/engine/DynamicsWorld.hpp>

#include "World.hpp"

using namespace sqt;
namespace maths = sq::maths;

struct World::Impl {
    rp3d::DynamicsWorld physicsWorld {{0, 0, -1}};
};


World::World(sq::MessageBus& _messageBus)
    : messageBus(_messageBus),
      entityManager(_messageBus),
      impl(new Impl()) {

    camera = std::make_unique<world::CameraObject>();

    impl->physicsWorld.setNbIterationsVelocitySolver(18u);
    impl->physicsWorld.setNbIterationsPositionSolver(12u);

    const auto configure_func = [this](auto _msg) {
        animationSystem.configure_entity(_msg.entity);
        _msg.entity->mark_dirty();
    };

    on_Configure_Entity.func = configure_func;

    on_Enable_SkyBox.func = [this](auto) { skybox = std::make_unique<world::SkyBoxObject>(); };
    on_Enable_Ambient.func = [this](auto) { ambient = std::make_unique<world::AmbientObject>(); };
    on_Enable_SkyLight.func = [this](auto) { skylight = std::make_unique<world::SkyLightObject>(); };

    on_Disable_SkyBox.func = [this](auto) { skybox.reset(nullptr); };
    on_Disable_Ambient.func = [this](auto) { ambient.reset(nullptr); };
    on_Disable_SkyLight.func = [this](auto) { skylight.reset(nullptr); };

    messageBus.subscribe_back(on_Configure_Entity);

    messageBus.subscribe_back(on_Enable_SkyBox);
    messageBus.subscribe_back(on_Enable_Ambient);
    messageBus.subscribe_back(on_Enable_SkyLight);

    messageBus.subscribe_back(on_Disable_SkyBox);
    messageBus.subscribe_back(on_Disable_Ambient);
    messageBus.subscribe_back(on_Disable_SkyLight);

    // temp debug stuff

    on_Debug_1.func = [=](auto) { soundSystem.play("effects/Whack", 1u); };
    on_Debug_2.func = [=](auto) { soundSystem.play("effects/Whack", 1u, Vec3F(0,0,5)); };
    messageBus.subscribe_back(on_Debug_1);
    messageBus.subscribe_back(on_Debug_2);
}

World::~World() = default;


world::StaticCell* World::add_StaticCell(const string& _name) {
    SQASSERT(!staticCellMap.count(_name), "world already has cell");
    return &staticCellMap[_name];
}

const world::StaticCell& World::get_StaticCell(const string& _name) const {
    SQASSERT(staticCellMap.count(_name), "world doesn't have cell");
    return staticCellMap.at(_name);
}


sq::EntityManager& World::get_EntityManager() { return entityManager; }
rp3d::DynamicsWorld& World::get_PhysicsWorld() { return impl->physicsWorld; }

const sq::EntityManager& World::get_EntityManager() const { return entityManager; }
const rp3d::DynamicsWorld& World::get_PhysicsWorld() const { return impl->physicsWorld; }


void World::update_options() {
    entityManager.configure_complete();
}

void World::tick() {
    //impl->rootEntity.propogate_forward<AnimatorComponent>    ( tick_wrapper );
    //impl->rootEntity.propogate_forward<DynamicBodyComponent> ( tick_wrapper );

    //impl->physicsWorld.update(1.f / 30.f);
}

void World::update() {

    auto refresh_wrapper = [this](sq::Entity* _entity) {
        animationSystem.refresh_entity(_entity);
        transformSystem.refresh_entity(_entity);
    };

    entityManager.propogate(refresh_wrapper);

    soundSystem.system_refresh();

    sceneData.staticCellMap = &staticCellMap;
    sceneData.camera = camera.get();
    sceneData.skybox = skybox.get();
    sceneData.ambient = ambient.get();
    sceneData.skylight = skylight.get();
}
