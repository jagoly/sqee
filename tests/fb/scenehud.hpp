#pragma once

#include <libsqee/base.hpp>

namespace sqt {

class SceneHud : public sq::Scene {
public:
    SceneHud(sq::Application*);

    void render(sf::RenderTarget&, float);
};

}
