#include <rp3d/collision/shapes/BoxShape.hpp>
#include <rp3d/engine/DynamicsWorld.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/misc/Files.hpp>

#include "SkyBox.hpp"
#include "Ambient.hpp"
#include "SkyLight.hpp"
#include "World.hpp"

using namespace sqt::wcoe;

World::~World() = default;


void World::update_stuff() {

}


World::World(sq::MessageBus& _messageBus, const sq::Settings& _settings, const sq::Camera* _camera)
    : root("root"), messageBus(_messageBus), settings(_settings), camera(_camera) {

    skybox.reset(new SkyBox(*this));
    ambient.reset(new Ambient(*this));
    skylight.reset(new SkyLight(*this));
    physWorld.reset(new rp3d::DynamicsWorld({0.f, 0.f, -1.f}));
    physWorld->setNbIterationsVelocitySolver(18u);
    physWorld->setNbIterationsPositionSolver(10u);

    //messageBus.register_type<sq::Entity*>("TransformComponentModified");
    //messageBus.register_type<sq::Entity*>("ModelComponentModified");
}


void World::refresh() {
    skybox->refresh();
    ambient->refresh();
    skylight->refresh();
}

void World::update() {
    skybox->update();
    ambient->update();
    skylight->update();

    physWorld->update(1.f / 24.f);
}

void World::calc(double _accum) {
    skybox->calc(_accum);
    ambient->calc(_accum);
    skylight->calc(_accum);
}
