#pragma once

#include <libsqee/base.hpp>

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application*);

    int seed;

    float pX, pY;
    float pVel;
    float pRot;
    int sSpeed;
    unsigned int dist;

    int newPipeCount;

    int flap;
    float flapCount;

    std::list<std::pair<int, int>> pipes;

    void setup_game(int);
    void add_pipe(int);

    void render(sf::RenderTarget&, float);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

}
