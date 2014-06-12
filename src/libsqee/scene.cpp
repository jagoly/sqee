#include <scene.hpp>
#include <application.hpp>

using namespace sq;

Scene::Scene(Application* _app) {
    app = _app;
    tickRate = 60;
    dt = 1.f / 60.f;
}

void Scene::render(sf::RenderTarget&, float) {
}

void Scene::update() {
}

