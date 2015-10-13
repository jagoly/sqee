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

World::World(const sq::Settings& _settings, const sq::Camera* _camera)
    : settings(_settings), camera(_camera) {

    skybox.reset(new SkyBox(*this));
    ambient.reset(new Ambient(*this));
    skylight.reset(new SkyLight(*this));
    physWorld.reset(new rp3d::DynamicsWorld({0.f, 0.f, -1.f}));
    physWorld->setNbIterationsVelocitySolver(18u);
    physWorld->setNbIterationsPositionSolver(10u);
}

Cell* World::add_cell(const string& _name) {
    Cell* ptr = new Cell(_name, *this);
    cellMap.emplace(_name, unique_ptr<Cell>(ptr));
    return cellMap.at(_name).get();
}

Cell* World::get_cell(const string& _name) {
    return cellMap.at(_name).get();
}

void World::reload_list() {
    objectList.clear();
    for (auto& sc : cellMap)
        if (sc.second->PROP_enabled)
            for (auto& so : sc.second->objectMap)
                objectList.push_back(so.second.get());
}

void World::invalidate() {
    for (auto& sc : cellMap)
        if (sc.second->PROP_enabled)
            sc.second->invalidate();
}

void World::refresh() {
    skybox->refresh();
    ambient->refresh();
    skylight->refresh();

    for (auto& sc : cellMap)
        if (sc.second->PROP_enabled)
            sc.second->refresh();

    reload_list();
}

void World::update() {
    skybox->update();
    ambient->update();
    skylight->update();

    for (auto& sc : cellMap)
        if (sc.second->PROP_enabled)
            sc.second->update();

    physWorld->update(1.f / 24.f);
}

void World::calc(double _accum) {
    skybox->calc(_accum);
    ambient->calc(_accum);
    skylight->calc(_accum);

    for (auto& sc : cellMap)
        if (sc.second->PROP_enabled)
            sc.second->calc(_accum);
}
