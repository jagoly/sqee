#pragma once

#include <libsqee/base.hpp>

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application*);

    float position;
    float nextPos;

    void render(sf::RenderTarget&, float);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

}
