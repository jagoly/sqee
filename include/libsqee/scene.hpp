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
    Scene(Application* _app);

    double accum;
    int tickRate = 60;
    double dt = 1.d/60.d;

    virtual void resize(unsigned int width, unsigned int height);
    virtual void update();
    virtual void render(sf::RenderTarget& target, float ft);
protected:
    Application* app;

    float interpolate(float a, float b);
};

typedef std::shared_ptr<Scene> ScenePtr;

}
