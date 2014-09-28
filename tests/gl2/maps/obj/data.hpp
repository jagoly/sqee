#pragma once

#include "object.hpp"

namespace sqt {
namespace obj {

class Data : public Object {
public:
    typedef std::shared_ptr<Data> Ptr;
    using Object::Object;
    void create();
};

}
}
