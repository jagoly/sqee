#pragma once

#include "Object.hpp"

namespace sqt { namespace wcoe {

class Liquid : public Object {
public:
    Liquid(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    // ANIMS
    void animate();
};

template<> struct ObjTraits<Liquid> {
    static constexpr ObjType type() { return ObjType::Liquid; }
};

}}
