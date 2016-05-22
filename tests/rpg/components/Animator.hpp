#pragma once

#include <map>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <sqee/ecs/Component.hpp>

namespace sqt {

template<class T, bool Norm> struct PropAnim {
    vector<T> values;
    vector<uint> times;
    bool repeat = false;
    uint progress = 0u;
    uint index = 0u;
    T prevValue;
};

class AnimatorComponent : public sq::Component
                        , public sq::ecs::TagUpdate
                        , public sq::ecs::TagTick {
 public:
    static string type() { return "Animator"; }

    // System Properties /////
    std::map<float*, PropAnim<float, 0>> floatMap;
    std::map<Vec2F*, PropAnim<Vec2F, 0>> vec2FMap;
    std::map<Vec3F*, PropAnim<Vec3F, 0>> vec3FMap;
    std::map<Vec4F*, PropAnim<Vec4F, 0>> vec4FMap;
    std::map<Vec2F*, PropAnim<Vec2F, 1>> norm2Map;
    std::map<Vec3F*, PropAnim<Vec3F, 1>> norm3Map;
    std::map<Vec4F*, PropAnim<Vec4F, 1>> norm4Map;
    std::map<QuatF*, PropAnim<QuatF, 1>> quatFMap;

    // Helper Functions /////
    template<class T, bool Norm> PropAnim<T, Norm>* FUNC_add_prop(T* _prop) {
        get_map<T, Norm>()[_prop] = PropAnim<T, Norm>();
        return &get_map<T, Norm>().at(_prop); }
    template<class T, bool Norm> PropAnim<T, Norm>* FUNC_get_prop(T* _prop) {
        return &get_map<T, Norm>().at(_prop); }
    template<class T, bool Norm> void FUNC_erase_prop(T* _prop) {
        get_map<T, Norm>().erase(_prop); }

 private:
    template<class T, bool Norm> std::map<T*, PropAnim<T, Norm>>& get_map();
};

template<> inline std::map<float*, PropAnim<float, 0>>& AnimatorComponent::get_map<float, 0>() { return floatMap; }
template<> inline std::map<Vec2F*, PropAnim<Vec2F, 0>>& AnimatorComponent::get_map<Vec2F, 0>() { return vec2FMap; }
template<> inline std::map<Vec3F*, PropAnim<Vec3F, 0>>& AnimatorComponent::get_map<Vec3F, 0>() { return vec3FMap; }
template<> inline std::map<Vec4F*, PropAnim<Vec4F, 0>>& AnimatorComponent::get_map<Vec4F, 0>() { return vec4FMap; }
template<> inline std::map<Vec2F*, PropAnim<Vec2F, 1>>& AnimatorComponent::get_map<Vec2F, 1>() { return norm2Map; }
template<> inline std::map<Vec3F*, PropAnim<Vec3F, 1>>& AnimatorComponent::get_map<Vec3F, 1>() { return norm3Map; }
template<> inline std::map<Vec4F*, PropAnim<Vec4F, 1>>& AnimatorComponent::get_map<Vec4F, 1>() { return norm4Map; }
template<> inline std::map<QuatF*, PropAnim<QuatF, 1>>& AnimatorComponent::get_map<QuatF, 1>() { return quatFMap; }

}
