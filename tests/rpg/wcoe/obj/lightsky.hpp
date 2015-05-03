#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class LightSky : public Object {
public:
    LightSky(const string& _name, const Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void update_from_data();

    void tick();

    struct {
        bool shadow = false;
        bool diffuse = false;
        bool specular = false;
        vec3 direction = {0, 0, -1};
        vec3 colour = {1, 1, 1};
        uint texsize = 0u;
    } DATA;

    unique_ptr<sq::LightSky> lSky;
    unique_ptr<sq::ShadowSky> sSky;
};

template<> struct ObjTraits<LightSky> {
    static constexpr ObjType type() { return ObjType::LightSky; }
};

}}
