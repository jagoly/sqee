#pragma once

#include <sqee/physics/RP3D.hpp>

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class RigBodyMulti : public Object {
public:
    RigBodyMulti(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    void animate();
};

template<> struct ObjTraits<RigBodyMulti> {
    static constexpr ObjType type() { return ObjType::RigBodyMulti; }
};

}}