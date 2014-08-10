#include <scene.hpp>
#include <application.hpp>

namespace sq {

Scene::Scene(Application* _app) {
    app = _app;
}

void Scene::resize(unsigned int, unsigned int) {
}

void Scene::render(sf::RenderTarget&, float) {
}

void Scene::update() {
}

float Scene::interpolate(float a, float b) {
    return a * ((dt - accum) / dt) + b * (accum / dt);
}

}
