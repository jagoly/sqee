#pragma once

#include <map>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

#include "Helpers.hpp"

namespace sqt {

template<class T, bool Norm> struct PropAnim {
    vector<T> values; vector<uint> times;
    uint progress = 0u; uint index = 0u;
    T prevValue; bool repeat = false;
};

class AnimatorComponent : public ComponentRPG
                        , public ecs::TagUpdate
                        , public ecs::TagTick {
public:
    AnimatorComponent(EntityRPG&, World&) {}
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
    PropAnim<float, 0>* FUNC_add_Float(float* _prop) { return &(floatMap[_prop] = PropAnim<float, 0>()); }
    PropAnim<Vec2F, 0>* FUNC_add_Vec2F(Vec2F* _prop) { return &(vec2FMap[_prop] = PropAnim<Vec2F, 0>()); }
    PropAnim<Vec3F, 0>* FUNC_add_Vec3F(Vec3F* _prop) { return &(vec3FMap[_prop] = PropAnim<Vec3F, 0>()); }
    PropAnim<Vec4F, 0>* FUNC_add_Vec4F(Vec4F* _prop) { return &(vec4FMap[_prop] = PropAnim<Vec4F, 0>()); }
    PropAnim<Vec2F, 1>* FUNC_add_Norm2(Vec2F* _prop) { return &(norm2Map[_prop] = PropAnim<Vec2F, 1>()); }
    PropAnim<Vec3F, 1>* FUNC_add_Norm3(Vec3F* _prop) { return &(norm3Map[_prop] = PropAnim<Vec3F, 1>()); }
    PropAnim<Vec4F, 1>* FUNC_add_Norm4(Vec4F* _prop) { return &(norm4Map[_prop] = PropAnim<Vec4F, 1>()); }
    PropAnim<QuatF, 1>* FUNC_add_QuatF(QuatF* _prop) { return &(quatFMap[_prop] = PropAnim<QuatF, 1>()); }
    PropAnim<float, 0>* FUNC_get_Float(float* _prop) { return &floatMap.at(_prop); }
    PropAnim<Vec2F, 0>* FUNC_get_Vec2F(Vec2F* _prop) { return &vec2FMap.at(_prop); }
    PropAnim<Vec3F, 0>* FUNC_get_Vec3F(Vec3F* _prop) { return &vec3FMap.at(_prop); }
    PropAnim<Vec4F, 0>* FUNC_get_Vec4F(Vec4F* _prop) { return &vec4FMap.at(_prop); }
    PropAnim<Vec2F, 1>* FUNC_get_Norm2(Vec2F* _prop) { return &norm2Map.at(_prop); }
    PropAnim<Vec3F, 1>* FUNC_get_Norm3(Vec3F* _prop) { return &norm3Map.at(_prop); }
    PropAnim<Vec4F, 1>* FUNC_get_Norm4(Vec4F* _prop) { return &norm4Map.at(_prop); }
    PropAnim<QuatF, 1>* FUNC_get_QuatF(QuatF* _prop) { return &quatFMap.at(_prop); }
    void FUNC_del_Float(float* _prop) { floatMap.erase(_prop); }
    void FUNC_del_Vec2F(Vec2F* _prop) { vec2FMap.erase(_prop); }
    void FUNC_del_Vec3F(Vec3F* _prop) { vec3FMap.erase(_prop); }
    void FUNC_del_Vec4F(Vec4F* _prop) { vec4FMap.erase(_prop); }
    void FUNC_del_Norm2(Vec2F* _prop) { norm2Map.erase(_prop); }
    void FUNC_del_Norm3(Vec3F* _prop) { norm3Map.erase(_prop); }
    void FUNC_del_Norm4(Vec4F* _prop) { norm4Map.erase(_prop); }
    void FUNC_del_QuatF(QuatF* _prop) { quatFMap.erase(_prop); }
};

}
