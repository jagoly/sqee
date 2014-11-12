#pragma once

#include "object.hpp"

namespace sqt {
namespace wld {

class Light : public Object {
public:
    using Object::Object;
    void create();
};

}
}
