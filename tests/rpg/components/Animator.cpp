//#include "../world/World.hpp"
//#include "Animator.hpp"

//namespace sqt {
//namespace maths = sq::maths;

//template<class T> inline T mix_and_normalize(const PropAnim<T, 0>& _anim, float _factor) {
//    return maths::mix(_anim.prevValue, _anim.values[_anim.index], _factor); }

//template<class T> inline T mix_and_normalize(const PropAnim<T, 1>& _anim, float _factor) {
//    return maths::normalize(maths::mix(_anim.prevValue, _anim.values[_anim.index], _factor)); }

//template<> void World::update_component(AnimatorComponent* _c, EntityRPG* _e) {
//    auto iterate_map_update = [this](auto& _propertyMap) -> bool {
//        for (auto& property : _propertyMap) { auto& anim = property.second;
//            float percent = (anim.progress + tickPercent) / anim.times[anim.index];
//            *property.first = mix_and_normalize(anim, std::min(percent, 1.f));
//        } return _propertyMap.empty() == false;
//    };

//    if (iterate_map_update(_c->floatMap) || iterate_map_update(_c->quatFMap) ||
//        iterate_map_update(_c->vec2FMap) || iterate_map_update(_c->norm2Map) ||
//        iterate_map_update(_c->vec3FMap) || iterate_map_update(_c->norm3Map) ||
//        iterate_map_update(_c->vec4FMap) || iterate_map_update(_c->norm4Map)) {
//        this->mark_refresh_component<TransformComponent>  (_e, true);
//        this->mark_refresh_component<StaticBodyComponent> (_e, true);
//        this->mark_refresh_component<ModelComponent>      (_e, true);
//        this->mark_refresh_component<DecalComponent>      (_e, true);
//        this->mark_refresh_component<SpotLightComponent>  (_e, true);
//        this->mark_refresh_component<PointLightComponent> (_e, true);
//        this->mark_refresh_component<ReflectComponent>    (_e, true);
//    }
//}

//template<> void World::tick_component(AnimatorComponent* _c, EntityRPG* _e) {
//    auto iterate_map_tick = [](auto& _propertyMap) -> bool {
//        if (_propertyMap.empty()) return false;
//        using MapT = std::decay_t<decltype(_propertyMap)>;
//        vector<typename MapT::key_type> removeVec;

//        for (auto& property : _propertyMap) {
//            auto& anim = property.second;
//            float percent = float(anim.progress) / anim.times[anim.index];
//            *property.first = mix_and_normalize(anim, percent);
//            if (anim.progress == anim.times[anim.index]) {
//                anim.prevValue = anim.values[anim.index];
//                anim.progress = 0u; anim.index += 1u;
//                if (anim.index == anim.values.size()) {
//                    if (anim.repeat == true) anim.index = 0u;
//                    else removeVec.push_back(property.first);
//                    *property.first = anim.values.back(); }
//            } else anim.progress += 1u; }

//        for (auto prop : removeVec) _propertyMap.erase(prop);
//        return _propertyMap.empty() == true;
//    };

//    if (iterate_map_tick(_c->floatMap) || iterate_map_tick(_c->quatFMap) ||
//        iterate_map_tick(_c->vec2FMap) || iterate_map_tick(_c->norm2Map) ||
//        iterate_map_tick(_c->vec3FMap) || iterate_map_tick(_c->norm3Map) ||
//        iterate_map_tick(_c->vec4FMap) || iterate_map_tick(_c->norm4Map)) {
//        this->mark_refresh_component<TransformComponent>  (_e, true);
//        this->mark_refresh_component<StaticBodyComponent> (_e, true);
//        this->mark_refresh_component<ModelComponent>      (_e, true);
//        this->mark_refresh_component<DecalComponent>      (_e, true);
//        this->mark_refresh_component<SpotLightComponent>  (_e, true);
//        this->mark_refresh_component<PointLightComponent> (_e, true);
//        this->mark_refresh_component<ReflectComponent>    (_e, true);
//    }
//}

//}
