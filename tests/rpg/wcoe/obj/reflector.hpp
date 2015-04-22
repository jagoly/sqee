#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class Reflector : public Object {
public:
    Reflector(const string& _name, const Cell& _cell);
    void load_from_spec(const ObjSpec& _spec);
};

}}
