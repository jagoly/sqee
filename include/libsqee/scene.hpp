#pragma once

#include <vector>
#include <string>
#include <memory>

#include <SFML/Graphics.hpp>

#include <textureholder.hpp>

namespace sq {

class Application;

class Scene {
public:
    Scene(Application*);

    Application* app;

    double accum = 0.f;
    int tickRate;
    double dt;

    //const sf::Texture& get_tex();

    virtual void update();
    virtual void render(sf::RenderTarget&, float);
protected:
    std::vector<sf::Font> fontVector;
};

}
