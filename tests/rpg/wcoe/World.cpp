#include <rp3d/engine/DynamicsWorld.hpp>

#include "../components/Animator.hpp"
#include "../components/DynamicBody.hpp"
#include "../components/Transform.hpp"
#include "../components/StaticBody.hpp"
#include "../components/Model.hpp"
#include "../components/Decal.hpp"
#include "../components/SpotLight.hpp"
#include "../components/PointLight.hpp"
#include "../components/Reflect.hpp"

#include "Camera.hpp"
#include "SkyBox.hpp"
#include "Ambient.hpp"
#include "SkyLight.hpp"
#include "World.hpp"

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/StringCast.hpp>

using namespace sqt;
namespace maths = sq::maths;

struct World::Impl {
    Camera camera;
    Ambient ambient;
    unique_ptr<SkyBox> skybox;
    unique_ptr<SkyLight> skylight;
    rp3d::DynamicsWorld physicsWorld {{0, 0, -1}};
    sq::Entity<World> rootEntity {"root"};
    std::set<const ecs::TagConfigure*> configureSet;
    std::set<const ecs::TagRefresh*> refreshSet;
};

World::World(RpgOptions& _options) : options(_options), impl(new Impl()) {
    impl->physicsWorld.setNbIterationsVelocitySolver(18u);
    impl->physicsWorld.setNbIterationsPositionSolver(12u);
}

World::~World() = default;


sq::Entity<World>& World::get_RootEntity() { return impl->rootEntity; }
rp3d::DynamicsWorld& World::get_PhysicsWorld() { return impl->physicsWorld; }
const sq::Entity<World>& World::get_RootEntity() const { return impl->rootEntity; }
const rp3d::DynamicsWorld& World::get_PhysicsWorld() const { return impl->physicsWorld; }

Camera& World::get_Camera() { return impl->camera; }
Ambient& World::get_Ambient() { return impl->ambient; }
const Camera& World::get_Camera() const { return impl->camera; }
const Ambient& World::get_Ambient() const { return impl->ambient; }

SkyBox& World::get_SkyBox() { SQASSERT(bool(impl->skybox), ""); return *impl->skybox; }
SkyLight& World::get_SkyLight() { SQASSERT(bool(impl->skylight), ""); return *impl->skylight; }
const SkyBox& World::get_SkyBox() const { SQASSERT(bool(impl->skybox), ""); return *impl->skybox; }
const SkyLight& World::get_SkyLight() const { SQASSERT(bool(impl->skylight), ""); return *impl->skylight; }

bool World::check_SkyBox() const { return impl->skybox.get() != nullptr; }
bool World::check_SkyLight() const { return impl->skylight.get() != nullptr; }
void World::enable_SkyBox(bool _enable) { if (check_SkyBox() != _enable) impl->skybox.reset(_enable ? new SkyBox() : nullptr); }
void World::enable_SkyLight(bool _enable) { if (check_SkyLight() != _enable) impl->skylight.reset(_enable ? new SkyLight() : nullptr); }


template<class T> void World::mark_configure_component(const EntityRPG* _e, bool _recursive) {
    static_assert(std::is_base_of<ecs::TagConfigure, T>::value, "");
    if (auto* c = _e->try_get_component<T>()) impl->configureSet.insert(c);
    if (_recursive == true) for (const auto& child : _e->get_children())
        mark_configure_component<T>(child.get(), true);
}

template<class T> void World::mark_refresh_component(const EntityRPG* _e, bool _recursive) {
    static_assert(std::is_base_of<ecs::TagRefresh, T>::value, "");
    if (auto* c = _e->try_get_component<T>()) impl->refreshSet.insert(c);
    if (_recursive == true) for (const auto& child : _e->get_children())
        mark_refresh_component<T>(child.get(), true);
}

void World::mark_configure_all_components(const EntityRPG* _e, bool _recursive) {
    mark_configure_component<ModelComponent>      (&impl->rootEntity, _recursive);
    mark_configure_component<DecalComponent>      (&impl->rootEntity, _recursive);
    mark_configure_component<SpotLightComponent>  (&impl->rootEntity, _recursive);
    mark_configure_component<PointLightComponent> (&impl->rootEntity, _recursive);
}

void World::mark_refresh_all_components(const EntityRPG* _e, bool _recursive) {
    mark_refresh_component<TransformComponent>  (&impl->rootEntity, _recursive);
    mark_refresh_component<StaticBodyComponent> (&impl->rootEntity, _recursive);
    mark_refresh_component<ModelComponent>      (&impl->rootEntity, _recursive);
    mark_refresh_component<DecalComponent>      (&impl->rootEntity, _recursive);
    mark_refresh_component<SpotLightComponent>  (&impl->rootEntity, _recursive);
    mark_refresh_component<PointLightComponent> (&impl->rootEntity, _recursive);
    mark_refresh_component<ReflectComponent>    (&impl->rootEntity, _recursive);
}

void World::clean_up_entity(EntityRPG* _e) {
    if (auto* c = _e->try_get_component<ModelComponent>()) clean_up_component(c, _e);
    if (auto* c = _e->try_get_component<DecalComponent>()) clean_up_component(c, _e);
    for (const auto& child : _e->get_children()) clean_up_entity(child.get());
}

void World::update_options() {
    if (bool(impl->skybox)) impl->skybox->configure(*this);
    if (bool(impl->skylight)) impl->skylight->configure(*this);
    mark_configure_all_components(&impl->rootEntity, true);
    mark_refresh_all_components(&impl->rootEntity, true);
}

void World::tick() {
    auto tick_wrapper = [this](auto* _c, EntityRPG* _e) { this->tick_component(_c, _e); };

    impl->rootEntity.propogate_forward<AnimatorComponent>    ( tick_wrapper );
    impl->rootEntity.propogate_forward<DynamicBodyComponent> ( tick_wrapper );

    impl->physicsWorld.update(1.f / 24.f);
}

void World::update() {
    impl->camera.update(); impl->ambient.update();
    if (bool(impl->skybox)) impl->skybox->update(*this);
    if (bool(impl->skylight)) impl->skylight->update(*this);

    auto configure_wrapper = [this](auto* _c, EntityRPG* _e) { if (impl->configureSet.count(_c)) this->configure_component(_c, _e); };
    auto refresh_wrapper   = [this](auto* _c, EntityRPG* _e) { if (impl->refreshSet.count(_c)) this->refresh_component(_c, _e); };
    auto update_wrapper    = [this](auto* _c, EntityRPG* _e) { this->update_component(_c, _e); };

    impl->rootEntity.propogate_forward<AnimatorComponent>    (                                     update_wrapper );
    impl->rootEntity.propogate_forward<DynamicBodyComponent> (                                     update_wrapper );
    impl->rootEntity.propogate_forward<TransformComponent>   (                    refresh_wrapper                 );
    impl->rootEntity.propogate_forward<StaticBodyComponent>  (                    refresh_wrapper                 );
    impl->rootEntity.propogate_forward<ModelComponent>       ( configure_wrapper, refresh_wrapper, update_wrapper );
    impl->rootEntity.propogate_forward<DecalComponent>       ( configure_wrapper, refresh_wrapper                 );
    impl->rootEntity.propogate_forward<SpotLightComponent>   ( configure_wrapper, refresh_wrapper                 );
    impl->rootEntity.propogate_forward<PointLightComponent>  ( configure_wrapper, refresh_wrapper                 );
    impl->rootEntity.propogate_forward<ReflectComponent>     (                    refresh_wrapper                 );
}
