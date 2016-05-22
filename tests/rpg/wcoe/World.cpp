#include <rp3d/collision/shapes/BoxShape.hpp>
#include <rp3d/engine/DynamicsWorld.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Resources.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/misc/Files.hpp>

#include "../components/Animator.hpp"
#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/Decal.hpp"
#include "../components/SpotLight.hpp"
#include "../components/PointLight.hpp"
#include "../components/Reflect.hpp"

#include "SkyBox.hpp"
#include "Ambient.hpp"
#include "SkyLight.hpp"
#include "World.hpp"

using namespace sqt;
namespace maths = sq::maths;

World::~World() = default;

World::World(sq::MessageBus& _messageBus, const sq::Settings& _settings, const sq::Camera& _camera)
    : root("root"), messageBus(_messageBus), settings(_settings), camera(_camera) {

    skybox.reset(new SkyBox());
    ambient.reset(new Ambient());
    skylight.reset(new SkyLight(*this));
    physWorld.reset(new rp3d::DynamicsWorld({0.f, 0.f, -1.f}));
    physWorld->setNbIterationsVelocitySolver(18u);
    physWorld->setNbIterationsPositionSolver(10u);
}

void World::update_entity(sq::Entity* _e) {
    const auto configure_refresh_update_component = [this](auto* _c, sq::Entity* _e) {
        configure_wrapper(_c, _e); refresh_wrapper(_c, _e); update_wrapper(_c, _e); };

    if (auto c = _e->get_component<AnimatorComponent>())   configure_refresh_update_component(c, _e);
    if (auto c = _e->get_component<TransformComponent>())  configure_refresh_update_component(c, _e);
    if (auto c = _e->get_component<ModelComponent>())      configure_refresh_update_component(c, _e);
    if (auto c = _e->get_component<DecalComponent>())      configure_refresh_update_component(c, _e);
    if (auto c = _e->get_component<SpotLightComponent>())  configure_refresh_update_component(c, _e);
    if (auto c = _e->get_component<PointLightComponent>()) configure_refresh_update_component(c, _e);
    if (auto c = _e->get_component<ReflectComponent>())    configure_refresh_update_component(c, _e);

    for (auto& child : _e->get_children()) update_entity(child.get());
}

void World::tick_entity(sq::Entity* _e) {
    if (auto c = _e->get_component<AnimatorComponent>()) tick_component(c, _e);

    for (auto& child : _e->get_children()) tick_entity(child.get());
}

void World::configure() {
    skybox->configure();
    ambient->configure();
    skylight->configure();

    root.mark_configure_all(true);
    root.mark_refresh_all(true);
}

void World::update() {
    skybox->update();
    ambient->update();
    skylight->update();

    update_entity(&root);
}

void World::tick() {
    tick_entity(&root);

    physWorld->update(1.f / 24.f);
}
