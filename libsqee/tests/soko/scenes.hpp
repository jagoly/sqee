#ifndef SCENES_HPP
#define SCENES_HPP

#include <libsqee/extra.hpp>

namespace sqt {

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

#endif // SCENES_HPP
