#include <rp3d/engine/DynamicsWorld.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Settings.hpp>

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

World::~World() = default;

struct World::Impl {
    std::set<const ecs::TagConfigure*> configureSet;
    std::set<const ecs::TagRefresh*> refreshSet;
};

World::World(sq::MessageBus& _messageBus, const sq::Settings& _settings)
    : root("root"), messageBus(_messageBus), settings(_settings), impl(new Impl()) {

    camera.reset(new Camera());
    skybox.reset(new SkyBox());
    ambient.reset(new Ambient());
    skylight.reset(new SkyLight(*this));
    physWorld.reset(new rp3d::DynamicsWorld({0.f, 0.f, -1.f}));
    physWorld->setNbIterationsVelocitySolver(18u);
    physWorld->setNbIterationsPositionSolver(12u);
}

template<class T> void World::mark_configure_component(const EntityRPG* _e, bool _recursive) {
    static_assert(std::is_base_of<ecs::TagConfigure, T>::value, "");
    if (auto c = _e->try_get_component<T>()) impl->configureSet.insert(c);
    if (_recursive == true) for (const auto& child : _e->get_children())
        mark_configure_component<T>(child.get(), true);
}

template<class T> void World::mark_refresh_component(const EntityRPG* _e, bool _recursive) {
    static_assert(std::is_base_of<ecs::TagRefresh, T>::value, "");
    if (auto c = _e->try_get_component<T>()) impl->refreshSet.insert(c);
    if (_recursive == true) for (const auto& child : _e->get_children())
        mark_refresh_component<T>(child.get(), true);
}

void World::mark_configure_all_components(const EntityRPG* _e, bool _recursive) {
    if (auto c = _e->try_get_component<ModelComponent>())      impl->configureSet.insert(c);
    if (auto c = _e->try_get_component<DecalComponent>())      impl->configureSet.insert(c);
    if (auto c = _e->try_get_component<SpotLightComponent>())  impl->configureSet.insert(c);
    if (auto c = _e->try_get_component<PointLightComponent>()) impl->configureSet.insert(c);
    if (_recursive == true) for (const auto& child : _e->get_children())
        mark_configure_all_components(child.get(), true);
}

void World::mark_refresh_all_components(const EntityRPG* _e, bool _recursive) {
    if (auto c = _e->try_get_component<TransformComponent>())  impl->refreshSet.insert(c);
    if (auto c = _e->try_get_component<StaticBodyComponent>()) impl->refreshSet.insert(c);
    if (auto c = _e->try_get_component<ModelComponent>())      impl->refreshSet.insert(c);
    if (auto c = _e->try_get_component<DecalComponent>())      impl->refreshSet.insert(c);
    if (auto c = _e->try_get_component<SpotLightComponent>())  impl->refreshSet.insert(c);
    if (auto c = _e->try_get_component<PointLightComponent>()) impl->refreshSet.insert(c);
    if (auto c = _e->try_get_component<ReflectComponent>())    impl->refreshSet.insert(c);
    if (_recursive == true) for (const auto& child : _e->get_children())
        mark_refresh_all_components(child.get(), true);
}

void World::clean_up_entity(EntityRPG* _e) {
    if (auto c = _e->try_get_component<ModelComponent>()) clean_up_component(c, _e);
    if (auto c = _e->try_get_component<DecalComponent>()) clean_up_component(c, _e);
}

void World::configure() {
    camera->configure(); skybox->configure();
    ambient->configure(); skylight->configure();

    this->mark_configure_component<ModelComponent>      (&root, true);
    this->mark_configure_component<DecalComponent>      (&root, true);
    this->mark_configure_component<SpotLightComponent>  (&root, true);
    this->mark_configure_component<PointLightComponent> (&root, true);

    this->mark_refresh_component<TransformComponent>  (&root, true);
    this->mark_refresh_component<StaticBodyComponent> (&root, true);
    this->mark_refresh_component<ModelComponent>      (&root, true);
    this->mark_refresh_component<DecalComponent>      (&root, true);
    this->mark_refresh_component<SpotLightComponent>  (&root, true);
    this->mark_refresh_component<PointLightComponent> (&root, true);
    this->mark_refresh_component<ReflectComponent>    (&root, true);
}

void World::update() {
    camera->update(); skybox->update();
    ambient->update(); skylight->update();

    auto configure_wrapper = [this](auto* _c, EntityRPG* _e) { if (impl->configureSet.count(_c)) this->configure_component(_c, _e); };
    auto refresh_wrapper   = [this](auto* _c, EntityRPG* _e) { if (impl->refreshSet.count(_c)) this->refresh_component(_c, _e); };
    auto update_wrapper    = [this](auto* _c, EntityRPG* _e) { this->update_component(_c, _e); };

    root.propogate_forward<AnimatorComponent>    (                                     update_wrapper );
    root.propogate_forward<DynamicBodyComponent> (                                     update_wrapper );
    root.propogate_forward<TransformComponent>   (                    refresh_wrapper                 );
    root.propogate_forward<StaticBodyComponent>  (                    refresh_wrapper                 );
    root.propogate_forward<ModelComponent>       ( configure_wrapper, refresh_wrapper, update_wrapper );
    root.propogate_forward<DecalComponent>       ( configure_wrapper, refresh_wrapper                 );
    root.propogate_forward<SpotLightComponent>   ( configure_wrapper, refresh_wrapper                 );
    root.propogate_forward<PointLightComponent>  ( configure_wrapper, refresh_wrapper                 );
    root.propogate_forward<ReflectComponent>     (                    refresh_wrapper                 );
}

void World::tick() {
    auto tick_wrapper = [this](auto* _c, EntityRPG* _e) { this->tick_component(_c, _e); };

    root.propogate_forward<AnimatorComponent>    ( tick_wrapper );
    root.propogate_forward<DynamicBodyComponent> ( tick_wrapper );

    physWorld->update(1.f / 24.f);
}
