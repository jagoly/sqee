#include <scene.hpp>
#include <application.hpp>

using namespace sq;

Scene::Scene(Application* _app) {
    app = _app;
    tickRate = 60;
    dt = 1.d / 60.d;
}

void Scene::resize(uint, uint) {
}

void Scene::render(sf::RenderTarget&, float) {
}

void Scene::update() {
}

float Scene::interpolate(float a, float b) {
    return a * ((dt - accum) / dt) + b * (accum / dt);
}
