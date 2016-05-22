#include "../wcoe/World.hpp"
#include "Animator.hpp"

namespace maths = sq::maths;

namespace sqt {

template<> void World::update_component(AnimatorComponent* _c, sq::Entity* _e) {
    for (auto& property : _c->floatMap) {
        PropAnim<float, 0>& anim = property.second;
        float percent = (anim.progress + accum * 24.0) / anim.times[anim.index];
        *property.first = maths::mix(anim.prevValue, anim.values[anim.index], std::min(percent, 1.f));
    }
}

template<> void World::tick_component(AnimatorComponent* _c, sq::Entity* _e) {
    auto iterate_map = [](auto& propertyMap) {
        using Type = typename std::remove_pointer_t<typename std::decay_t<decltype(propertyMap)>::key_type>;
        std::vector<Type*> removeVec;
        for (auto& property : propertyMap) {
            auto& anim = property.second;
            float percent = float(anim.progress) / anim.times[anim.index];
            *property.first = maths::mix(anim.prevValue, anim.values[anim.index], percent);
            if (anim.progress == anim.times[anim.index]) {
                anim.prevValue = anim.values[anim.index];
                anim.progress = 0u; anim.index += 1u;
                if (anim.index == anim.values.size()) {
                    if (anim.repeat == true) anim.index = 0u;
                    else removeVec.push_back(property.first);
                    *property.first = anim.values.back(); }
            } else anim.progress += 1u; }
        for (auto prop : removeVec) propertyMap.erase(prop);
    };

    iterate_map(_c->floatMap); iterate_map(_c->quatFMap);
    iterate_map(_c->vec2FMap); iterate_map(_c->norm2Map);
    iterate_map(_c->vec3FMap); iterate_map(_c->norm3Map);
    iterate_map(_c->vec4FMap); iterate_map(_c->norm4Map);
}

}
