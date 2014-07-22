#pragma once

#include <list>

#include <libsqee/scene.hpp>
#include <libsqee/handler.hpp>

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application*);

    unsigned int seed;
    std::minstd_rand gen;

    bool crashed;

    float pX, pY;
    float pVel;
    float pRot;
    int speed;
    int dist;

    unsigned int score;

    unsigned int flap;

    float fOffset;
    unsigned int fOffsetCount;

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
