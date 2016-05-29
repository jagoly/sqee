#pragma once

#include <sqee/builtins.hpp>

namespace sq {

// Forward Declaration /////
template<class WorldType> class Entity;

template<class WorldType>
class Component : sq::NonCopyable {
public:
    virtual ~Component() = default;
    using EntityType = Entity<WorldType>;
};

}
