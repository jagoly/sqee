#pragma once

#include <sqee/builtins.hpp>
#include <sqee/ecs/Entity.hpp>

namespace sqt {

class TransformSystem {
public:

    void refresh_entity(sq::Entity* _entity);
};

}
