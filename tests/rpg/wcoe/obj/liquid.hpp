#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class Liquid : public Object {
public:
    Liquid(const string& _name, const Cell& _cell);
    void load_from_spec(const ObjSpec& _spec);
};

template<> struct ObjTraits<Liquid> {
    static constexpr ObjType type() { return ObjType::Liquid; }
};

}}
