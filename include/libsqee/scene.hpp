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

    virtual void update();
    virtual void render(sf::RenderTarget&, float);
protected:
    Application* app;

    std::vector<sf::Font> fontVector;

    template<typename T>
    T interpolate(T, T);
};

typedef std::shared_ptr<Scene> ScenePtr;

}
