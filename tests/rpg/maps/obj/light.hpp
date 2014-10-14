#pragma once

#include "object.hpp"

namespace sqt {
namespace obj {

class Light : public Object {
public:
    using Object::Object;
    void create();
};

}
}
