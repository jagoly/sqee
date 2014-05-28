#ifndef SUITE_HPP
#define SUITE_HPP

#include <libsqee/extra.hpp>
#include <libsqee/tests/soko/level.hpp>

namespace sqt {

class StageMain : public sq::Stage {
public:
    StageMain();
    void update();

    int pDir, pX, pY, pMoved;
    float pSpeed;
    sf::Vector2u size;
    std::vector<std::pair<unsigned short int, std::string>> texPathVec;
    std::vector<std::vector<unsigned short int>> levelTexVec;
    std::vector<std::vector<unsigned short int>> levelObjVec;

    void load_level(Level&);
};

class StageHud : public sq::Stage {
public:
    StageHud();
    void update();
};

}

#endif // SUITE_HPP
