#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class LightPoint : public Object {
public:
    LightPoint(const string& _name, const Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void update_from_data();

    struct {
        bool shadow = false;
        bool diffuse = false;
        bool specular = false;
        vec3 position = {0, 0, 0};
        vec3 colour = {1, 1, 1};
        float intensity = 10.f;
        uint texsize = 0u;
    } DATA;

    unique_ptr<sq::LightPoint> lPoint;
    unique_ptr<sq::ShadowPoint> sPoint;
    sq::Sphere sphere; array<sq::Frustum, 6> frusArr;
};

template<> struct ObjTraits<LightPoint> {
    static constexpr ObjType type() { return ObjType::LightPoint; }
};

}}
