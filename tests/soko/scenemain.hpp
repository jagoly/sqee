#pragma once

#include <libsqee/base.hpp>
#include "level.hpp"

namespace sqt {

class SceneMain : public sq::Scene {
public:
    SceneMain(sq::Application*);

    short int pFace, pDir, pX, pY, pMoved;
    float pSpeed;
    sf::Vector2i size;
    sf::Vector2i pushing;
    std::vector<std::pair<unsigned short int, std::string>> texPathVec;
    std::vector<std::vector<unsigned short int>> levelTexVec;
    std::vector<std::vector<unsigned short int>> levelObjVec;

    void load_level(Level&);

    void render(sf::RenderTarget&, float);
    void update();
};

}
