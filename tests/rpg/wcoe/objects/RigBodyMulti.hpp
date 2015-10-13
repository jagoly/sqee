#pragma once

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class RigBodyMulti : public Object {
public:
    RigBodyMulti(const string& _name, Cell* _cell);

    void load_from_spec(const ObjSpec& _spec);

    void refresh(), update();
    void calc(double _accum);
    void animate();
};

}}
