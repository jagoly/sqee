#ifndef SUITE_HPP
#define SUITE_HPP

#include <libsqee/extra.hpp>

namespace sqt {

class StageMain : public sq::Stage {
public:
    StageMain();
    void update();
};
class StageHud : public sq::Stage {
public:
    StageHud();
    void update();
};

}

#endif // SUITE_HPP
