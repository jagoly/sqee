#ifndef SCENEHUD_HPP
#define SCENEHUD_HPP

#include <libsqee/base.hpp>

namespace sqt {

class SceneHud : public sq::Scene {
public:
    SceneHud(sq::Application*);

    short int pX, pY;

    void update();
    void render(sf::RenderTarget&, float);
};

}

#endif // SCENEHUD_HPP
