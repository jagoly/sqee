#pragma once

#include "object.hpp"

namespace sqt {
namespace obj {

class Data : public Object {
public:
    using Object::Object;
    void create();
};

}
}
