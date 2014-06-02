#ifndef SCENES_HPP
#define SCENES_HPP

#include <libsqee/extra.hpp>
#include "stages.hpp"

namespace sqt {

class SceneMain : public sq::Scene {
public:
    SceneMain(sq::Stage&, sq::TextureHolder&);
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
