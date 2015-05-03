#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class LightSpot : public Object {
public:
    LightSpot(const string& _name, const Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void update_from_data();

    struct {
        bool shadow = false;
        bool diffuse = false;
        bool specular = false;
        vec3 position = {0, 0, 0};
        vec3 direction = {0, 0, -1};
        vec3 colour = {1, 1, 1};
        float angle = 45.f;
        float intensity = 10.f;
        float softness = 0.f;
        uint texsize = 0u;
    } DATA;

    unique_ptr<sq::LightSpot> lSpot;
    unique_ptr<sq::ShadowSpot> sSpot;
    sq::Sphere sphere; sq::Frustum frus;
};

template<> struct ObjTraits<LightSpot> {
    static constexpr ObjType type() { return ObjType::LightSpot; }
};

}}
