#include <scene.hpp>
#include <application.hpp>

using namespace sq;

Scene::Scene(Application* _app) {
    app = _app;
    tickRate = 60;
    dt = 1 / 60.d;
}

void Scene::render(sf::RenderTarget&, float) {
}

void Scene::update() {
}

template<typename T>
T Scene::interpolate(T a, T b) {
    return a * ((dt - accum) / dt) + b * (accum / dt);
}
