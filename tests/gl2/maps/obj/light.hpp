#pragma once

#include "object.hpp"

namespace sqt {
namespace obj {

class Light : public Object {
public:
    typedef std::shared_ptr<Light> Ptr;
    using Object::Object;
    void create();
};

}
}
