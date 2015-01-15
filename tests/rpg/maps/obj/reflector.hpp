#pragma once

#include "object.hpp"

namespace sqt {

class Reflector : public Object {
public:
    using Object::Object;
    void create();
};

}
