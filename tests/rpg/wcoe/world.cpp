#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sqee/app/logging.hpp>
#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/maths.hpp>
#include <sqee/misc/files.hpp>

#include "world.hpp"

using namespace sqt::wcoe;

void World::add_cell(const string& _name, vec3 _position) {
    Cell* ptr = new Cell(_name, _position, *this);
    cellMap.emplace(_name, shared_ptr<Cell>(ptr));
}

void World::add_cell(const string& _name, vec3 _position, const string& _path) {
    add_cell(_name, _position);
    cellMap.at(_name)->load_from_file(_path);
}

void World::enable_cell(const string& _cell) {
    for (const auto& so : cellMap.at(_cell)->objMap) {
        objectList.remove_if([so](const weak_ptr<Object>& val) {
            return val.lock().get() == so.second.get(); });
        objectList.emplace_front(so.second);
    }
}

void World::disable_cell(const string& _cell) {}

bool World::get_cell_enabled(const string& _cell) {}

void World::tick() {
    for (auto& sc : cellMap) sc.second->tick();
}

void World::calc(double _accum) {
    for (auto& sc : cellMap) sc.second->calc(_accum);
}
