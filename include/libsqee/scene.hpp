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

    double accum;
    int tickRate;
    double dt;

    virtual void resize(uint, uint);
    virtual void update();
    virtual void render(sf::RenderTarget&, float);
protected:
    Application* app;

    std::vector<sf::Font> fontVector;

    float interpolate(float, float);
};

typedef std::shared_ptr<Scene> ScenePtr;

}
