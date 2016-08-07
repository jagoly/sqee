#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/ecs/Entity.hpp>

namespace sq {

class EntityManager : NonCopyable {
public:
    EntityManager(MessageBus& _messageBus);

    Entity* create_root(const string& _name);

    Entity* create(Entity* _parent, const string& _name);

    void configure(Entity* _entity, bool _recursive);

    void destroy(Entity* _entity);

    void set_name(Entity* _entity, const string& _name);

    Entity* find(const string& _name);

    void configure_complete();

    void clear_dirty_flags();

    /// Propogate a function to all entities
    template<class Func> void propogate(Func&& _func) {
        for (auto& e : rootEntities) e->propogate(_func);
    }

private:
    MessageBus& messageBus;

    vector<unique_ptr<Entity>> rootEntities;
    std::unordered_map<string, Entity*> uniqueNames;
};

}
