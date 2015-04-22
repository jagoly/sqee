#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class LightSky : public Object {
public:
    LightSky(const string& _name, const Cell& _cell);
    void load_from_spec(const ObjSpec& _spec);

    void tick();

    unique_ptr<sq::LightSky> sky;
};

template<> struct ObjTraits<LightSky> {
    static constexpr ObjType type() { return ObjType::LightSky; }
};

}}
