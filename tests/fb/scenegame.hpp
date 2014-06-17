#pragma once

#include <libsqee/base.hpp>

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application*);

    uint seed;
    std::minstd_rand gen;

    bool crashed;

    float pX, pY;
    float pVel;
    float pRot;
    int speed;
    int dist;

    uint score;

    uint flap;

    float fOffset;
    uint fOffsetCount;

    std::list<std::pair<int, int>> pipes;

    void setup_game();
    void add_pipe();
    void update_crashed();
    void end_game();

    void render(sf::RenderTarget&, float);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

}
