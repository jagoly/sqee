#pragma once

#include "../Object.hpp"

namespace sqt { namespace wcoe {

class Liquid final : public Object {
public:
    Liquid(const string& _name, Cell* _cell);

    void load_from_spec(const ObjSpec& _spec);

    void refresh(), update();
    void calc(double _accum);
    void animate();
};

}}
