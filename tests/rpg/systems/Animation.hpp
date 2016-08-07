#pragma once

#include <sqee/builtins.hpp>
#include <sqee/ecs/Entity.hpp>

namespace sqt {

class AnimationSystem {
public:

    AnimationSystem();
    ~AnimationSystem();

    float tickPercent = 0.f;

    void configure_entity(sq::Entity* _entity);
    void refresh_entity(sq::Entity* _entity);
    void remove_entity(sq::Entity* _entity);

private:
    struct Impl;
    friend struct Impl;
    unique_ptr<Impl> impl;
};

}
