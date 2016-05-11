#pragma once

#include <sqee/builtins.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/ecs/Entity.hpp>

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

    void update_stuff();
    sq::Entity root;

    unique_ptr<SkyBox> skybox;
    unique_ptr<Ambient> ambient;
    unique_ptr<SkyLight> skylight;

    sq::MessageBus& messageBus;
    const sq::Settings& settings;
    const sq::Camera* const camera;

    unique_ptr<rp3d::DynamicsWorld> physWorld;
};

}}
