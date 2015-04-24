#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class LightPoint : public Object {
public:
    LightPoint(const string& _name, const Cell& _cell);
    void load_from_spec(const ObjSpec& _spec);

    void tick();

    unique_ptr<sq::LightPoint> point;
    bool shadow = false, diffuse = false, specular = false;
};

template<> struct ObjTraits<LightPoint> {
    static constexpr ObjType type() { return ObjType::LightPoint; }
};

}}
