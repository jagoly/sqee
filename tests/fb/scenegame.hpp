#pragma once

#include <libsqee/base.hpp>

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application*);

    bool active;
    void render(sf::RenderTarget&, float);
    void update();
};

}
