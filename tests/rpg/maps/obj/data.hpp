#pragma once

#include "object.hpp"

namespace sqt {
namespace wld {

class Data : public Object {
public:
    using Object::Object;
    void create();
};

}
}
