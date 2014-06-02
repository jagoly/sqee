#ifndef SUITE_HPP
#define SUITE_HPP

#include <libsqee/extra.hpp>
#include "level.hpp"

namespace sqt {

class StageMain : public sq::Stage {
public:
    StageMain();

    short int pFace, pDir, pX, pY, pMoved;
    float pSpeed;
    sf::Vector2i size;
    sf::Vector2i pushing;
    std::vector<std::pair<unsigned short int, std::string>> texPathVec;
    std::vector<std::vector<unsigned short int>> levelTexVec;
    std::vector<std::vector<unsigned short int>> levelObjVec;

    void load_level(Level*);
    void update();
};

class StageHud : public sq::Stage {
public:
    StageHud();

    short int pX, pY;

    void update();
};

}

#endif // SUITE_HPP
