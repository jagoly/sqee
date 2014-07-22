#pragma once

#include <libsqee/scene.hpp>
#include "level.hpp"

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application*);

    bool active;
    std::unique_ptr<Level> level;
    int pFace, pDir, pX, pY, pMoved;
    float pSpeed;
    sf::Vector2i size;
    sf::Vector2i pushing;
    std::vector<std::pair<unsigned short int, std::string>> texPathVec;
    std::vector<std::vector<unsigned short int>> levelTexVec;
    std::vector<std::vector<unsigned short int>> levelObjVec;

    void load_level(std::string);
    void start_level();
    void win_level();

    void render(sf::RenderTarget&, float);
    void update();
};

}
