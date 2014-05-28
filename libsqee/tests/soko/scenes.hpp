#ifndef SCENES_HPP
#define SCENES_HPP

#include <libsqee/extra.hpp>
#include <libsqee/tests/soko/stages.hpp>

namespace sqt {

class SceneBack : public sq::Scene {
public:
    SceneBack(sq::Stage&);

    void render(float);
    StageMain* stage;
};
class SceneFore : public sq::Scene {
public:
    SceneFore(sq::Stage&);

    void render(float);
    StageMain* stage;
};
class SceneHud : public sq::Scene {
public:
    SceneHud(sq::Stage&);

    void render(float);
    StageHud* stage;
};

}

#endif // SCENES_HPP
