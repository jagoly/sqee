#ifndef SCENES_HPP
#define SCENES_HPP

#include <libsqee/extra.hpp>
#include <libsqee/tests/soko/stages.hpp>

namespace sqt {

class SceneBack : public sq::Scene {
public:
    SceneBack(sq::Stage&, sq::TextureHolder&);
    StageMain* stage;
    void render(float);    
};
class SceneFore : public sq::Scene {
public:
    SceneFore(sq::Stage&, sq::TextureHolder&);
    StageMain* stage;
    void render(float);
};
class SceneHud : public sq::Scene {
public:
    SceneHud(sq::Stage&, sq::TextureHolder&);
    StageHud* stage;
    void render(float);
};

}

#endif // SCENES_HPP
