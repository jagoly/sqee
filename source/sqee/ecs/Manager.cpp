#include <sqee/ecs/Manager.hpp>
#include <sqee/messages.hpp>

using namespace sq;

EntityManager::EntityManager(MessageBus& _messageBus)
    : messageBus(_messageBus) {}

Entity* EntityManager::create_root(const string& _name) {

    // create the new root entity in place
    rootEntities.push_back(std::make_unique<Entity>(nullptr));
    Entity* entity = rootEntities.back().get();

    if (_name.empty() == false) {
        // optionally set the unique name of the entity
        SQASSERT(!uniqueNames.count(_name), "name already used");
        uniqueNames.insert({_name, entity});
        entity->uniqueName = _name; }

    // send entity creation message
    messageBus.send_message(msg::Create_Entity{entity});

    return entity;
}

Entity* EntityManager::create(Entity* _parent, const string& _name) {

    // create the new child entity in place
    _parent->children.push_back(std::make_unique<Entity>(_parent));
    Entity* entity = _parent->children.back().get();

    if (_name.empty() == false) {
        // optionally set the unique name of the entity
        SQASSERT(!uniqueNames.count(_name), "name already used");
        uniqueNames.insert({_name, entity});
        entity->uniqueName = _name; }

    // send entity creation message
    messageBus.send_message(msg::Create_Entity{entity});

    return entity;
}

void EntityManager::configure(sq::Entity* _entity, bool _recursive) {

    // send a configure entity message
    messageBus.send_message(msg::Configure_Entity{_entity});

    // optionally configure all of the entity's children
    if (_recursive) for (auto& c : _entity->children) configure(c.get(), true);
}

void EntityManager::destroy(Entity* _entity) {

    // destroy all of the entity's children
    for (auto& c : reverse(_entity->children)) destroy(c.get());

    // send a destroy entity message
    messageBus.send_message(msg::Destroy_Entity{_entity});

    // get a reference to the correct siblings vector and find the iter
    auto& siblings = _entity->parent ? _entity->parent->children : rootEntities;
    auto predicate = [_entity](const auto& c) { return c.get() == _entity; };
    auto iter = algo::find_if(siblings.begin(), siblings.end(), predicate);

    SQASSERT(iter != siblings.end(), "entity or not part of family");

    siblings.erase(iter);
}

void EntityManager::set_name(Entity* _entity, const string& _name) {

    if ((_entity->uniqueName).empty() == false) {
        // optionally remove the entity's old unique name
        SQASSERT(uniqueNames.count(_entity->uniqueName), "name not used");
        uniqueNames.erase(_entity->uniqueName); }

    if (_name.empty() == false) {
        // optionally set the entity's new unique name
        SQASSERT(!uniqueNames.count(_name), "name already used");
        uniqueNames.insert({_name, _entity}); }

    _entity->uniqueName = _name;
}

Entity* EntityManager::find(const string& _name) {
    const auto iter = uniqueNames.find(_name);
    SQASSERT(iter != uniqueNames.end(), "name not found");
    return iter->second;
}

void EntityManager::configure_complete() {

    // recursively configure all root entities
    for (auto& c : rootEntities) configure(c.get(), true);
}

void EntityManager::clear_dirty_flags() {

    auto clear_func = [](Entity* e) { e->dirtyFlag = false; };
    for (auto& e : rootEntities) e->propogate(clear_func);
}
