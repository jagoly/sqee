#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class LightSpot : public Object {
public:
    LightSpot(const string& _name, const Cell& _cell);
    void load_from_spec(const ObjSpec& _spec);

    void tick();

    unique_ptr<sq::LightSpot> spot;
    bool shadow = false, diffuse = false, specular = false;
};

template<> struct ObjTraits<LightSpot> {
    static constexpr ObjType type() { return ObjType::LightSpot; }
};

}}
