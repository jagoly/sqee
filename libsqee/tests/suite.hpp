#ifndef SUITE_HPP
#define SUITE_HPP

#include <libsqee/extra.hpp>

namespace sqt {

class TestApp : public sq::Application {
public:
    TestApp();
};

class HandlerGame : public sq::Handler {
public:
    bool handle(sf::Event&);
};


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


class SceneBack : public sq::Scene {
public:
    void render(float);
};
class SceneFore : public sq::Scene {
public:
    void render(float);
};
class SceneHud : public sq::Scene {
public:
    SceneHud();
    void render(float);
};

}

#endif // SUITE_HPP
