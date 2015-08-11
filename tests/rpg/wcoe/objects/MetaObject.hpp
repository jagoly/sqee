#pragma once

#include <chaiscript/dispatchkit/boxed_value.hpp>

#include "../Object.hpp"

namespace sqt { namespace wcoe {

class MetaObject : public Object {
public:
    MetaObject(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    void animate();
};

template<> struct ObjTraits<MetaObject> {
    static constexpr ObjType type() { return ObjType::MetaObject; }
};

}}
